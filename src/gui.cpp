#include <opencv2/opencv.hpp>
#include "gui.h"

using namespace cv;

void check_pointer(void* ptr) {
	if (ptr == NULL) {
		ERROR("Fatal error: %s\n", SDL_GetError());
		exit(5);
	}
}

int gui_window::calibrate() {
	int board_width = 7;
	int board_height = 5;
	double mar = 0.5;
	namedWindow("Video Player");//Declaring the video to show the video//
	VideoCapture cap(0);//Declaring an object to capture stream of frames from default camera//
	if (!cap.isOpened()){ //This section prompt an error message if no video stream is found//
		cout << "No video stream detected" << endl;
		system("pause");
		return-1;
	}
	cap.set(CAP_PROP_FRAME_WIDTH,1920);
	cap.set(CAP_PROP_FRAME_HEIGHT,1080);

	// SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
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
			SDL_Rect rect;
			rect.x = 10;
			rect.y = 10;
			rect.w = 20;
			rect.h = 20;
			SDL_RenderFillRect(sdl_renderer, &rect);
			SDL_RenderPresent( sdl_renderer );
		}

		imshow("Video Player", myImage);//Showing the video//
		char c = (char)waitKey(1);//Allowing 25 milliseconds frame processing time and initiating break condition//
		if (c == 27){ //If 'Esc' is entered break the loop//
			break;
		}
	}
	printf("%d %d\n",(int) corners.size().width,(int) corners.size().height);


	Size target_size(window_width,window_height);
	std::vector<cv::Point3f> object_points;
	double square_size = target_size.width/(board_width+1);
	for (int i = 0; i < board_height; ++i) {
		for (int j = 0; j < board_width; ++j) {
			object_points.push_back(cv::Point3f(check_x[j+1], check_y[i+1], 0));
		}
	}

	Mat H = findHomography(object_points, corners);
	while (true) {
		Mat myImage, warped_image;
		cap >> myImage;
		warpPerspective(myImage, warped_image, H, target_size,WARP_INVERSE_MAP);
		imshow("Video Player", warped_image);//Showing the video//
		char c = (char)waitKey(1);//Allowing 25 milliseconds frame processing time and initiating break condition//
		if (c == 27){ //If 'Esc' is entered break the loop//
			break;
		}
	}

}


gui_window::gui_window(int window_width_, int window_height_, Solver * solver_) : solver(solver_), window_width(window_width_), window_height(window_height_) {
    output("Initializing SDL window\n");
    // sdl_window = SDL_CreateWindow("Graphical Window", SDL_WINDOWPOS_UNDEFINED_DISPLAY(1), SDL_WINDOWPOS_UNDEFINED_DISPLAY(1), sx, sy, SDL_WINDOW_FULLSCREEN);
    sdl_window = SDL_CreateWindow("Graphical Window", 0, 0, window_width, window_height, 0);
	check_pointer(sdl_window);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
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
	srcrect.w = solver->region.nx;
	srcrect.h = solver->region.ny;
	srcrect.x = 0;
	srcrect.y = 0;

	double sx = (double) window_width / solver->region.nx;
	double sy = (double) window_height / solver->region.ny;
	double s = sx;
	if (sy > s) s = sy;
	dstrect.w = s * solver->region.nx;
	dstrect.h = s * solver->region.ny;
	dstrect.x = 0;
	dstrect.x = (window_width - dstrect.w)/2;
	dstrect.y = (window_height - dstrect.h)/2;

	calibrate();

}

int gui_window::eventloop() {
	SDL_Event event;
    int ret = 0;
	solver->lattice->Color(outputBitmap); // Updating graphics
	SDL_LockSurface(sdl_surface);
	CudaMemcpy(sdl_surface->pixels, outputBitmap, sizeof(uchar4)*solver->region.sizeL(), cudaMemcpyDeviceToHost);
	SDL_UnlockSurface(sdl_surface);
	
	SDL_UpdateTexture(sdl_texture, NULL, sdl_surface->pixels, sdl_surface->pitch);

	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_RenderCopy(sdl_renderer, sdl_texture, &srcrect, &dstrect);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(sdl_renderer, 0, 0, 200, 200);
    SDL_RenderPresent( sdl_renderer );

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

gui_window::~gui_window() {
	CudaFree( outputBitmap );
    output("Killing SDL window\n");
}

