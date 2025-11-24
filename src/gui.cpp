#include <opencv2/opencv.hpp>
#include "gui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "TripleBuf.hpp"

using namespace cv;

void check_pointer(void* ptr) {
	if (ptr == NULL) {
		ERROR("Fatal error: %s\n", SDL_GetError());
		exit(5);
	}
}

class gui_window_implementation {
    Solver * solver;
    int window_width;
    int window_height;
	int window_scale;
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
	TripleBuf< Mat > frame_buf;
	int calibrate();
	Vec3d dir, mean;
	double lower, upper;
	Mat GetBin(bool show=false);
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

Mat gui_window_implementation::GetBin(bool show) {
	Mat camImage, myImage;
	cap >> camImage;
	if (show) imshow("Video Player", camImage);

	Mat gray(camImage.size(), CV_8U);
	Mat upp(camImage.size(), CV_8U);
	Mat low(camImage.size(), CV_8U);
	for(int x=0; x<camImage.rows; x++) {
		for(int y=0; y<camImage.cols; y++) {
			Vec3d v = camImage.at<Vec3b>(x, y);
			v = v - mean;
			double val = v.dot(dir);
			gray.at<unsigned char>(x, y) = val+128;
			if (val < lower) {
				low.at<unsigned char>(x, y) = 255;
			} else {
				low.at<unsigned char>(x, y) = 0;
			}
			if (val > upper) {
				upp.at<unsigned char>(x, y) = 255;
			} else {
				upp.at<unsigned char>(x, y) = 0;
			}
		}
	}

	if (show) {
		Mat myImage;
		warpPerspective(gray, myImage, H, target_size,WARP_INVERSE_MAP);
		imshow("Color", myImage);
	}
	Mat bin;
	warpPerspective(low, bin, H, target_size,WARP_INVERSE_MAP);
	if (show) {
		imshow("Lower", bin);
	}
	return bin;
}


int gui_window_implementation::calibrate() {
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
        	if (elapsed.count() > 2) break;
		} else {
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
	for (int ix = 0; ix<=board_width; ix++) {
		for (int iy = 0; iy<=board_height; iy++) {
			int a = 255*ix/board_width;
			int b = 255*iy/board_height;
			SDL_SetRenderDrawColor(sdl_renderer, a,0,b, 255);
			SDL_Rect rect;
			rect.x = check_x[ix];
			rect.y = check_y[iy];
			rect.w = check_x[ix+1] - check_x[ix];
			rect.h = check_y[iy+1] - check_y[iy];
			SDL_RenderFillRect(sdl_renderer, &rect);
		}
	}
    SDL_RenderPresent( sdl_renderer );
	waitKey(1000);

	Matx33d cov;
	double total = 0;
	mean = Vec3d::zeros();
	cov = Matx33d::zeros();
	auto gather_start = std::chrono::steady_clock::now();
	while (true) {
		Mat camImage, myImage;
		cap >> camImage;
		warpPerspective(camImage, myImage, H, target_size,WARP_INVERSE_MAP);
		for(int x=0; x<myImage.rows; x++) {
			for(int y=0; y<myImage.cols; y++) {
				Vec3d v = myImage.at<Vec3b>(x, y);
				total++;
				for (int i=0; i<3; i++) {
					mean(i) += v[i];
					for (int j=0; j<3; j++) cov(i,j) += v[i]*v[j];
				}
			}
		}
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - gather_start);
		if (elapsed.count() > 2) break;
	}

	mean = mean / total;
	cov = cov / total;
	for (int i=0; i<3; i++) for (int j=0; j<3; j++) cov(i,j) = cov(i,j) - mean(i)*mean(j);

	int idx = 2;
	{
		Vec3d val;
		Matx33d vec, vect;
		SVDecomp(cov, val, vec, vect);
		output("sd = [ %5lg, %5lg, %5lg ]\n",sqrt(val(0)),sqrt(val(1)),sqrt(val(2)));
		dir = Vec3d(vec(0,idx),vec(1,idx),vec(2,idx));
		double mx = 0;
		for (int i=0; i<3;i++) if (fabs(dir(i)) > fabs(mx)) mx = dir(i);
		if (mx < 0) dir = -dir;
		output("dir = [ %5lg, %5lg, %5lg ]\n",dir(0),dir(1),dir(2));
		double sd = sqrt(val[idx]);
		lower = -sd*2.5;
		upper =  sd*2.5;
	}

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
	//cap.set(CAP_PROP_AUTOFOCUS, 0);
	cap.set(CAP_PROP_BUFFERSIZE, 1);
	cap.set(CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 30);

	SDL_Init(SDL_INIT_VIDEO);

	fullscreen = false;
	display = 0;
    if (SDL_GetNumVideoDisplays() > 1) {
        display = 1;
		fullscreen = true;
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
		sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

	CudaMalloc( &outputBitmap, sizeof(uchar4)*solver->region.sizeL());
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
		printf("%d %d %d %d\n",(int)subreg.x,(int)subreg.y,(int)subreg.w,(int)subreg.h);
		target_size.width = subreg.w;
		target_size.height = subreg.h;
	}

	calibrate();
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


	Mat binImage, labelImage;
	binImage = GetBin(false);

	Mat stats, centroids;
	int nLabels = connectedComponentsWithStats(binImage, labelImage, stats, centroids);

	vector<int> index(nLabels-1, 0);
	for (int i = 0; i < nLabels-1; i++) index[i] = i + 1;

	const int CV_AREA = ConnectedComponentsTypes::CC_STAT_AREA;
	size_t idx_b = 0xFFFFFF;
	int idx_b_max = 0;
	for (size_t i=1;i<nLabels;i++) {
		int area = stats.at<int>(i,CV_AREA);
		if (area > idx_b_max) {
			idx_b = i;
			idx_b_max = area;
		}
	}

	Vec3b col0(0, 0, 0);
	Vec3b col1(0, 0, 255);
	Vec3b col2(0, 255, 0);
	Mat dst(binImage.size(), CV_8UC3);
	for(int r = 0; r < dst.rows; ++r){
		for(int c = 0; c < dst.cols; ++c){
			int label = labelImage.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			if (label == idx_b) {
				pixel = col1;
			} if (label == 0) {
				pixel = col0;
			} else {
				pixel = col2;
			}
			int x = c + subreg.x;
			int y = r + subreg.y;
			if (x - subreg.x < subreg.w && y - subreg.y < subreg.h) {
				size_t off = reg.offset(x,y);
				if (label == 0) {
					nodetypes[off] = nodetypes_save[off];
				} else {
					nodetypes[off] = NODE_Wall;
				}
			}
		}
	}
	//imshow("Video Player", dst);
	waitKey(1);

	solver->lattice->FlagOverwrite(nodetypes.data(),reg);

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

