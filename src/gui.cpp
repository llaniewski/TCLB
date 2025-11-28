#include <opencv2/opencv.hpp>
#include "gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <thread>
#include <latch>
#include "WaitFreeTripleBuf.hpp"

using namespace cv;

void check_pointer(void* ptr) {
	if (ptr == NULL) {
		ERROR("Fatal error: %s\n", SDL_GetError());
		exit(5);
	}
}

const int least_sq_size = 9;

class gui_window_implementation {
    Solver * solver;
    int window_width;
    int window_height;
	double window_scale;
	int display;
	bool fullscreen;
	SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	SDL_Texture* sdl_display;
	SDL_Texture *sdl_texture;
	SDL_Surface* sdl_surface;
	uchar4* outputBitmap;
	SDL_Rect srcrect, dstrect;
	cv::VideoCapture cap;
	cv::Size target_size;
	cv::Mat H;
	lbRegion reg;
	SDL_Rect subreg;
	std::vector<flag_t> nodetypes_save;
	std::vector<flag_t> nodetypes;
	wf3b::WaitFreeTripleBuf< Mat > camBuf;
	std::jthread camThread, sdlThread;
	int calibrate(bool show = true);
	Mat dark;
	Matx<double, least_sq_size, 1> filter_coef;
	double lower, upper;
	Mat getLabelImage(bool show);
	void camLoop(std::stop_token st);
	void sdlLoop(std::stop_token st);
public:
	gui_window_implementation(int window_width_, int window_height_, Solver * solver_);
	int eventloop();
	~gui_window_implementation();
};

gui_window::gui_window(int window_width_, int window_height_, Solver * solver_) :
	impl(new gui_window_implementation(window_width_, window_height_, solver_)) {
}

int gui_window::eventloop() {
	return impl->eventloop();
}

gui_window::~gui_window() {
	if (impl) delete impl;
}

int gui_window_implementation::calibrate(bool show) {
	int board_width = 14;
	int board_height = 7;
	double mar = 0.5;
	SDL_SetRenderDrawColor(sdl_renderer, 128, 128, 128, 255);
	SDL_RenderClear(sdl_renderer);
	std::vector<double> check_x, check_y;
	for (int ix = 0; ix<=board_width+1; ix++) check_x.push_back((ix + mar) * window_width / (board_width+1+2*mar));
	for (int iy = 0; iy<=board_height+1; iy++) check_y.push_back((iy + mar) * window_height / (board_height+1+2*mar));
	for (int ix = 0; ix<=board_width; ix++) {
		for (int iy = 0; iy<=board_height; iy++) {
			if ((ix+iy) % 2 == 0) {
				SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
			} else {
				SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
			}
			SDL_Rect rect;
			rect.x = check_x[ix];
			rect.y = check_y[iy];
			rect.w = check_x[ix+1] - check_x[ix];
			rect.h = check_y[iy+1] - check_y[iy];
			SDL_RenderFillRect(sdl_renderer, &rect);
		}
	}
    SDL_RenderPresent( sdl_renderer );

	Mat corners;
	int state = 0;
	SDL_Rect rect;
	rect.x = 0.2*mar * window_width / (board_width+1+2*mar);
	rect.y = 0.2*mar * window_height / (board_height+1+2*mar);
	rect.w = 0.6*mar * window_width / (board_width+1+2*mar);
	rect.h = 0.6*mar * window_height / (board_height+1+2*mar);
	auto last_unknown = std::chrono::steady_clock::now();
	while (true) {
		Mat myImage, gray;
		Mat corn;
		cap >> myImage;
		
		cvtColor(myImage, gray, COLOR_BGR2GRAY);
		bool ret = findChessboardCorners(gray, Size(board_width,board_height), corn);
		if (ret) {
			cornerSubPix(gray, corn, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.1));
			drawChessboardCorners(myImage, Size(board_width,board_height), corn, ret);
			corners = corn;
			state = 2;
		} else {
			last_unknown = std::chrono::steady_clock::now();
		}
		{
			if (state == 1) {
				SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 0, 255);
			} else if (state == 2) {
				SDL_SetRenderDrawColor(sdl_renderer, 0, 255, 0, 255);
				state = 1;
			} else {
				SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
			}
			SDL_RenderFillRect(sdl_renderer, &rect);
			SDL_RenderPresent( sdl_renderer );
		}

		if (fullscreen) {
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_unknown);
        	if (elapsed.count() > 1) break;
		}
		if (show) {
			imshow("Video Player", myImage);
			char c = (char)waitKey(1);
			if (c == 27) break;
		}
	}
	printf("%d %d\n",(int) corners.size().width,(int) corners.size().height);


	std::vector<cv::Point3f> object_points;
	double square_size = target_size.width/(board_width+1);
	printf("%d %d\n",(int) check_x.size(),(int) check_y.size());
	printf("%d %d\n",(int) board_width,(int) board_height);
	for (int iy = 1; iy<=board_height; iy++) {
		for (int ix = 1; ix<=board_width; ix++) {
			object_points.push_back(cv::Point3f(
				(check_x[ix] - dstrect.x)/window_scale - subreg.x,
				(check_y[iy] - dstrect.y)/window_scale - subreg.y,
			0));
		}
	}
	
	H = findHomography(object_points, corners);
	

	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
    SDL_RenderPresent( sdl_renderer );

	{
		auto start = std::chrono::steady_clock::now();
		while (true) {
			Mat camImage, myImage;
			cap >> camImage;
			warpPerspective(camImage, dark, H, target_size,WARP_INVERSE_MAP);
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
			if (elapsed.count() > 1) break;
		}
	}
	//imshow("Dark",dark);

	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);

	for (int ix = 0; ix<=board_width; ix++) {
		for (int iy = 0; iy<=board_height; iy++) {
			// int a = 255*ix/board_width;
			// int b = 255*iy/board_height;
			int a = rand() % 256;
			int b = rand() % 256;
			//int b = 0;
			SDL_SetRenderDrawColor(sdl_renderer, a,0,b, 255);
			SDL_Rect rect;
			rect.x = check_x[ix];
			rect.y = check_y[iy];
			rect.w = check_x[ix+1] - check_x[ix];
			rect.h = check_y[iy+1] - check_y[iy];
			SDL_RenderFillRect(sdl_renderer, &rect);
		}
	}
	// for (int x = 0; x<window_width; x++) {
	// 	for (int y = 0; y<=window_height; y++) {
	// 		int a = (1-cos(8 * 3.1415 * x/window_width))/2*255;
	// 		int b = (1-cos(8 * 3.1415 * y/window_height))/2*255;
	// 		SDL_SetRenderDrawColor(sdl_renderer, a,0,b, 255);
	// 		SDL_RenderDrawPoint(sdl_renderer,x,y);
	// 	}
	// }
    SDL_RenderPresent( sdl_renderer );
	waitKey(1000);

	{
		
		// Mat XTX(least_sq_size,least_sq_size,CV_64F);
		// Mat XTY(least_sq_size,1,CV_64F);
		Matx<double, least_sq_size,least_sq_size> XTX;
		Matx<double, least_sq_size, 1> XTY;
		
		for (int i=0; i<least_sq_size; i++) {
			XTY(i,0) = 0;
			for (int j=0; j<least_sq_size; j++) {
				XTX(i,j) = 0;
			}
		}
		auto gather_start = std::chrono::steady_clock::now();
		while (true) {
			Mat camImage, myImage;
			cap >> camImage;
			warpPerspective(camImage, myImage, H, target_size,WARP_INVERSE_MAP);

			for(int x=0; x<myImage.rows; x++) {
				for(int y=0; y<myImage.cols; y++) {
					Vec3d v1 = myImage.at<Vec3b>(x, y);
					Vec3d v2 = dark.at<Vec3b>(x, y);
					Vec3d v = v1 - v2;
					double g = v[1];
					double a;
					a = v[0]/255;
					double r[] = {a*a, (1-a)*(1-a),2*a*(1-a)};
					a = v[2]/255;
					double b[] = {a*a, (1-a)*(1-a),2*a*(1-a)};
					double all[] = {r[0]*b[0],r[1]*b[0],r[2]*b[0],
									r[0]*b[1],r[1]*b[1],r[2]*b[1],
									r[0]*b[2],r[1]*b[2],r[2]*b[2]};
					for (int i=0; i<least_sq_size; i++) {
						XTY(i,0) += all[i] * g;
						for (int j=0; j<least_sq_size; j++) {
							XTX(i,j) += all[i] * all[j];
						}
					}
				}
			}
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - gather_start);
			bool ret = solve(XTX, XTY, filter_coef);
			if (ret) {
				Mat gray(myImage.size(), CV_8U);
				Mat green(myImage.size(), CV_8U);
				double mean = 0;
				double var_g = 0;
				double var_gr = 0;
				double total = 0;
				for(int x=0; x<myImage.rows; x++) {
					for(int y=0; y<myImage.cols; y++) {
						Vec3d v1 = myImage.at<Vec3b>(x, y);
						Vec3d v2 = dark.at<Vec3b>(x, y);
						Vec3d v = v1 - v2;
						double g = v[1];
						double a;
						a = v[0]/255;
						double r[] = {a*a, (1-a)*(1-a),2*a*(1-a)};
						a = v[2]/255;
						double b[] = {a*a, (1-a)*(1-a),2*a*(1-a)};
						double all[] = {r[0]*b[0],r[1]*b[0],r[2]*b[0],
										r[0]*b[1],r[1]*b[1],r[2]*b[1],
										r[0]*b[2],r[1]*b[2],r[2]*b[2]};
						double gr = g;
						for (int i=0; i<least_sq_size; i++) {
							gr -= all[i] * filter_coef(i,0);
						}
						gray.at<unsigned char>(x,y) = gr + 128;
						green.at<unsigned char>(x,y) = g+128;
						total += 1;
						mean += g;
						var_g += g*g;
						var_gr += gr*gr;
					}
				}
				mean = mean / total;
				var_g = var_g / total - mean*mean;
				var_gr = var_gr / total;
				printf("mean: %6lg, sd: %6lg, after filter: %6lg\n",mean, sqrt(var_g), sqrt(var_gr));
				// imshow("Color", myImage);
				// imshow("Gray", gray);
				// imshow("Green", green);
			}
		// char c = (char)waitKey(1);
		// if (c == 27){ 
		// 	break;
		// }
			if (elapsed.count() > 1) break;
		}
	}

	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
    SDL_RenderPresent( sdl_renderer );
	waitKey(1000);
	// while (true) {
	// 	Mat camImage, myImage;
	// 	cap >> camImage;
	// 	imshow("Video Player", camImage);
	// 	Mat gray(camImage.size(), CV_8UC1);
	// 	for(int x=0; x<camImage.rows; x++) {
	// 		for(int y=0; y<camImage.cols; y++) {
	// 			Vec3d v = camImage.at<Vec3b>(x, y);
	// 			v = v - mean;
	// 			gray.at<unsigned char>(x, y) = v.dot(dir) + 128;
	// 		}
	// 	}

	// 	warpPerspective(gray, myImage, H, target_size,WARP_INVERSE_MAP);

	// 	imshow("Color", myImage);
	// 	Mat bin;
	// 	threshold(myImage, bin, lower, 255, THRESH_BINARY_INV);
	// 	imshow("Lower", bin);
	// 	threshold(myImage, bin, upper, 255, THRESH_BINARY);
	// 	imshow("Upper", bin);
	// 	char c = (char)waitKey(1);
	// 	if (c == 27){ 
	// 		break;
	// 	}
	// }

	return 0;
}




gui_window_implementation::gui_window_implementation(int window_width_, int window_height_, Solver * solver_)
	: solver(solver_), window_width(window_width_), window_height(window_height_), cap(0,CAP_V4L2) {
    output("Initializing SDL window\n");

	reg = solver->lattice->region;
	// <?R if ("Wall" %in% NodeTypes$name) { ?>
	// 	flag_t NodeType = NODE_Wall;
	// 	data->lattice->FlagOverwrite(&NodeType,r); // Overwrite mesh flags with flags from 'mask' table
	// <?R } ?>

	if (!cap.isOpened()){ //This section prompt an error message if no video stream is found//
		cout << "No video stream detected" << endl;
		exit(-1);
	}
	cap.set(CAP_PROP_FRAME_WIDTH,1920);
	cap.set(CAP_PROP_FRAME_HEIGHT,1080);
	cap.set(CAP_PROP_BUFFERSIZE, 1);
	cap.set(CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 30);

//                            gain 0x00980913 (int)    : min=0 max=255 step=1 default=0 value=0 flags=has-min-max
	cap.set(CAP_PROP_GAIN, 0);
//         white_balance_automatic 0x0098090c (bool)   : default=1 value=1
	cap.set(CAP_PROP_AUTO_WB, 0);
//            power_line_frequency 0x00980918 (menu)   : min=0 max=2 default=2 value=2 (60 Hz)
//       white_balance_temperature 0x0098091a (int)    : min=2000 max=6500 step=1 default=4000 value=5044 flags=inactive, has-min-max
	cap.set(CAP_PROP_WB_TEMPERATURE, 6500);
//                       sharpness 0x0098091b (int)    : min=0 max=255 step=1 default=128 value=128 flags=has-min-max
	cap.set(CAP_PROP_SHARPNESS, 0);
//          backlight_compensation 0x0098091c (int)    : min=0 max=1 step=1 default=0 value=0 flags=has-min-max
// Camera Controls
//                   auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=3 (Aperture Priority Mode)
	cap.set(CAP_PROP_AUTO_EXPOSURE, 1);
//          exposure_time_absolute 0x009a0902 (int)    : min=3 max=2047 step=1 default=250 value=83 flags=inactive, has-min-max
	cap.set(CAP_PROP_EXPOSURE, 250);
//      exposure_dynamic_framerate 0x009a0903 (bool)   : default=0 value=1
//                    pan_absolute 0x009a0908 (int)    : min=-36000 max=36000 step=3600 default=0 value=0 flags=has-min-max
//                   tilt_absolute 0x009a0909 (int)    : min=-36000 max=36000 step=3600 default=0 value=0 flags=has-min-max
//                  focus_absolute 0x009a090a (int)    : min=0 max=250 step=5 default=0 value=0 flags=inactive, has-min-max
	cap.set(CAP_PROP_FOCUS, 0);
//      focus_automatic_continuous 0x009a090c (bool)   : default=1 value=1
	cap.set(CAP_PROP_AUTOFOCUS, 0);
//                   zoom_absolute 0x009a090d (int)    : min=100 max=500 step=1 default=100 value=100 flags=has-min-max


	SDL_Init(SDL_INIT_VIDEO);


	fullscreen = false;
	display = 0;
    if (SDL_GetNumVideoDisplays() > 1) {
        display = 1;
		fullscreen = true;
    }
	if (fullscreen) {
		SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	}
    SDL_Rect bounds;
    SDL_GetDisplayBounds(display, &bounds);

	if (!fullscreen) {
		if (bounds.w > window_width) bounds.w = window_width;
		if (bounds.h > window_height) bounds.h = window_height;
	}
	output("Using display %d: %dx%d at (%d, %d)\n", display, bounds.w, bounds.h, bounds.x, bounds.y);
	if (fullscreen) {
		sdl_window = SDL_CreateWindow("Graphical Window", bounds.x, bounds.y, bounds.w, bounds.h, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
		// sdl_window = SDL_CreateWindow("Graphical Window", SDL_WINDOWPOS_UNDEFINED_DISPLAY(1), SDL_WINDOWPOS_UNDEFINED_DISPLAY(1), sx, sy, SDL_WINDOW_FULLSCREEN);
	} else {
    	sdl_window = SDL_CreateWindow("Graphical Window", bounds.x, bounds.y, bounds.w, bounds.h, SDL_WINDOW_ALLOW_HIGHDPI);
	}
	check_pointer(sdl_window);
	window_width = bounds.w;
	window_height = bounds.h;

	if (fullscreen) {
		//sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
	} else {
		sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
	}
	check_pointer(sdl_renderer);
	sdl_display = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window_width, window_height);
	check_pointer(sdl_display);
	//SDL_SetRenderTarget(sdl_renderer, sdl_display);

	sdl_surface = SDL_CreateRGBSurface( 0, solver->region.nx, solver->region.ny, 32,
												0xFF000000,
												0x00FF0000,
												0x0000FF00,
												0x000000FF
												);
	check_pointer(sdl_surface);
	sdl_texture = SDL_CreateTexture(sdl_renderer,SDL_PIXELFORMAT_ABGR8888,
												SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET,
												solver->region.nx, solver->region.ny);
	check_pointer(sdl_texture);

	CudaMalloc( &outputBitmap, sizeof(uchar4)*reg.sizeL());
	srcrect.w = reg.nx;
	srcrect.h = reg.ny;
	srcrect.x = 0;
	srcrect.y = 0;

	double sx = (double) window_width / reg.nx;
	double sy = (double) window_height / reg.ny;
	window_scale = sx;
	if (sy > window_scale) window_scale = sy;
	dstrect.w = window_scale * reg.nx;
	dstrect.h = window_scale * reg.ny;
	dstrect.x = (window_width - dstrect.w)/2;
	dstrect.y = (window_height - dstrect.h)/2;
	{
		SDL_Rect A;
		A.w = window_width / window_scale;
		A.h = window_height / window_scale;
		A.x = -(window_width / window_scale - reg.nx)/2;
		A.y = -(window_height / window_scale - reg.ny)/2;
		SDL_IntersectRect(&A, &srcrect, &subreg);
		target_size.width = subreg.w;
		target_size.height = subreg.h;
	}

	
	//std::latch calibrated(2);
	output("Calibrating...\n");
	calibrate(false);
	output("Calibrated\n");
	destroyAllWindows();
    // camThread = std::jthread{[this](std::stop_token st) {	
	// 	//calibrated.arrive_and_wait();
	// 	camLoop(st);
	// }};
	// //calibrated.arrive_and_wait();
	// sdlThread = std::jthread{[this](std::stop_token st) { sdlLoop(st); }};
}

Mat gui_window_implementation::getLabelImage(bool show) {
	Mat camImage, myImage;
	cap >> camImage;
	warpPerspective(camImage, myImage, H, target_size,WARP_INVERSE_MAP | INTER_NEAREST, BORDER_CONSTANT, 0);
	Mat gray(myImage.size(), CV_8U);
	Mat low(myImage.size(), CV_8U);
	for(int x=0; x<myImage.rows; x++) {
		for(int y=0; y<myImage.cols; y++) {
			Vec3d v1 = myImage.at<Vec3b>(x, y);
			Vec3d v2 = dark.at<Vec3b>(x, y);
			Vec3d v = v1 - v2;
			double g = v[1];
			double a;
			a = v[0]/255;
			double r[] = {a*a, (1-a)*(1-a),2*a*(1-a)};
			a = v[2]/255;
			double b[] = {a*a, (1-a)*(1-a),2*a*(1-a)};
			double all[] = {r[0]*b[0],r[1]*b[0],r[2]*b[0],
							r[0]*b[1],r[1]*b[1],r[2]*b[1],
							r[0]*b[2],r[1]*b[2],r[2]*b[2]};
			double gr = g;
			for (int i=0; i<least_sq_size; i++) {
				gr -= all[i] * filter_coef(i,0);
			}
			gray.at<unsigned char>(x,y) = gr + 128;
			if (gr < -20) {
				low.at<unsigned char>(x, y) = 255;
			} else {
				low.at<unsigned char>(x, y) = 0;
			}
		}
	}
	Mat binImage = low;
	//Canny(gray, bin, 50,60);
	Mat labelImage, stats, centroids;
	int nLabels = connectedComponentsWithStats(binImage, labelImage, stats, centroids, 8, CV_32S);
	const int CV_AREA = ConnectedComponentsTypes::CC_STAT_AREA;
	const int area_limit = 100;
	int idx_b = 0;
	int idx_b_max = 0;
	for (size_t i=1;i<nLabels;i++) {
		int area = stats.at<int>(i,CV_AREA);
		if (area < area_limit) continue;
		if (area > idx_b_max) {
			idx_b = i;
			idx_b_max = area;
		}
	}
	for (int r = 0; r < labelImage.rows; ++r) {
		for (int c = 0; c < labelImage.cols; ++c) {
			int label = labelImage.at<int>(r, c);
			int type;
			if (label == 0) {
				type = 0;
			} else if (label == idx_b) {
				type = 1;
			} else if (label > 0) {
				int area = stats.at<int>(label,CV_AREA);
				if (area < area_limit) {
					type = 3;
				} else {
					type = 2;
				}
			}
			label = type;
		}
	}
	if (show) {
		Vec3b col0(0, 0, 0);
		Vec3b col1(255, 255, 255);
		Vec3b col2(0, 255, 0);
		Vec3b col3(255, 0, 0);
		Mat dst(Size(binImage.cols*2,binImage.rows*2), CV_8UC3);
		for(int r = 0; r < binImage.rows; ++r){
			for(int c = 0; c < binImage.cols; ++c){
				{
					int type = labelImage.at<int>(r, c);
					Vec3b &pixel = dst.at<Vec3b>(r, c);
					if  (type == 0) {
						pixel = col0;
					} else if  (type == 1) {
						pixel = col1;
					} else if  (type == 2) {
						pixel = col2;
					} else {
						pixel = col3;
					}
				}
				{
					Vec3b &pixel = dst.at<Vec3b>(binImage.rows+r, c);
					Vec3b &color = myImage.at<Vec3b>(r, c);
					pixel = color;
				}
				{
					Vec3b &pixel = dst.at<Vec3b>(r, binImage.cols+c);
					int v = gray.at<unsigned char>(r, c);
					pixel = Vec3b(v,v,v);
				}
			}
		}
		// imshow("Color", myImage);
		// imshow("Gray", gray);
		// imshow("Binary", bin);
		imshow("Comp", dst);
		waitKey(1);
	}
	return labelImage;
}


void gui_window_implementation::camLoop(std::stop_token st) {
	bool show = true;
	while (not st.stop_requested()) {
		Mat labelImage = getLabelImage(show);
		camBuf.produce([&labelImage](std::span< Mat > data) { data[0] = labelImage.clone(); });
	}
	printf("Finishing camloop\n");
}

void gui_window_implementation::sdlLoop(std::stop_token st) {
	
}


int gui_window_implementation::eventloop() {
	SDL_Event event;
    int ret = 0;
	
	if (nodetypes.size() == 0) {
		nodetypes.resize(reg.sizeL());
		nodetypes_save.resize(reg.sizeL());
		solver->lattice->GetFlags(reg, nodetypes.data());
		solver->lattice->GetFlags(reg, nodetypes_save.data());
	}

	solver->lattice->Color(outputBitmap); // Updating graphics
	SDL_LockSurface(sdl_surface);
	CudaMemcpy(sdl_surface->pixels, outputBitmap, sizeof(uchar4)*solver->region.sizeL(), cudaMemcpyDeviceToHost);
	SDL_UnlockSurface(sdl_surface);
	
	SDL_UpdateTexture(sdl_texture, NULL, sdl_surface->pixels, sdl_surface->pitch);

	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderCopy(sdl_renderer, sdl_texture, &srcrect, &dstrect);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	//SDL_RenderDrawLine(sdl_renderer, 0, 0, 200, 200);
    SDL_RenderPresent( sdl_renderer );

	// for (Mat& labelImage : camBuf.consume()) {
	{
		Mat labelImage = getLabelImage(true);
		for(int r = 0; r < labelImage.rows; ++r){
			for(int c = 0; c < labelImage.cols; ++c){
				int type = labelImage.at<int>(r, c);
				int x = c + subreg.x;
				int y = r + subreg.y;
				if (x - subreg.x < subreg.w && y - subreg.y < subreg.h) {
					size_t off = reg.offset(x,y);
					if (type == 1 or type == 2) {
						nodetypes[off] = NODE_Wall;
					} else {
						nodetypes[off] = nodetypes_save[off];
					}
				}
			}
		}
		solver->lattice->FlagOverwrite(nodetypes.data(),reg);
	}

	while( SDL_PollEvent(&event) )
	{
		if (event.type == SDL_QUIT) {
            exit(0);
			ret = 1;
		}
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
            exit(0);
			ret = 1;
		}
	}
	
    return 0;
}

gui_window_implementation::~gui_window_implementation() {
	cap.release();
	CudaFree( outputBitmap );
    output("Killing SDL window\n");
}

