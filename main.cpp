#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include <bpf_tools/distproxyserver.h>

#include <commonpp/mainappcontext.h>

using namespace std;
using namespace cv;

// Object structure
struct Object {
	Mat image;
	int x;
	int y;
	int vx;
	int vy;
	int width;
	int height;
};

#include <QDebug>
#include <QElapsedTimer>

static int genvideo()
{
	// Load background image
	Mat background = imread("background.jpg");
	if (background.empty()) {
		cout << "Failed to load background image" << endl;
		return -1;
	}

	// Create video writer to save frames as video
	Size frame_size(background.cols, background.rows);
	VideoWriter video_writer("animation.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, frame_size);

	// Define parameters for objects
	int num_objects = 2;                // Number of objects to overlay
	int max_objects = 10;                // Maximum number of objects to overlay in a frame
	int max_size_ratio = 3;             // Maximum size ratio of objects compared to background
	int num_frames = 500;               // Number of frames to generate
	vector<Object> objects;            // Vector to store objects

	// Load object images and initialize object properties
	for (int i = 0; i < num_objects; i++) {
		Object obj;
		string filename = "object" + to_string(i + 1) + ".png"; // Object image filename
		obj.image = imread(filename, -1); // Load object image with alpha channel

		// Set initial position within the boundaries of the background image
		obj.width = obj.image.cols;
		obj.height = obj.image.rows;
		obj.x = rand() % (background.cols - obj.width + 1);
		obj.y = rand() % (background.rows - obj.height + 1);

		// Set random velocity for object movement
		obj.vx = rand() % 5 - 2; // Random horizontal velocity (-2 to 2)
		obj.vy = rand() % 5 - 2; // Random vertical velocity (-2 to 2)

		objects.push_back(obj); // Add object to vector
	}

	// Randomly pick 0 to N objects to overlay in the frame
	std::vector<int> objectsSelected;
	int num_overlay = rand() % (max_objects + 1);
	for (int j = 0; j < num_overlay; j++) {
		int obj_idx = rand() % num_objects; // Randomly pick an object from the vector
		objectsSelected.push_back(obj_idx);
	}

	// Generate frames
	for (int i = 0; i < num_frames; i++) {
		Mat frame = background.clone();

		for (auto idx: objectsSelected) {
			Object& obj = objects[idx]; // Get reference to the object

			// Update object position based on velocity
			obj.x += obj.vx;
			obj.y += obj.vy;

			/*
			obj.vx = std::max(obj.vx, 0);
			obj.vx = std::min(obj.vx, frame.cols - obj.width);
			obj.vy = std::max(obj.vy, 0);
			obj.vy = std::min(obj.vy, frame.rows - obj.height);
			*/
			// Check boundaries and bounce back if object goes out of bounds
			if (obj.x < 0 || obj.x > frame.cols - obj.width) {
				obj.x = 0;
			}
			if (obj.y < 0 || obj.y > frame.rows - obj.height) {
				obj.y = 0;
			}

			// Overlay object image on frame
			obj.image.copyTo(frame(Rect(obj.x, obj.y, obj.width, obj.height)));
		}

		// Add frame to video writer
		video_writer.write(frame);
	}

	// Release video writer
	video_writer.release();

	return 0;
}

extern int workerMain(MainAppContext &ctx);
extern int valsimMain(MainAppContext &ctx);

int main(int argc, char *argv[])
{
	auto ctx = MainAppContext::parse(argc, argv);
	ctx.addbin("valsim", valsimMain);
	ctx.addbin("valsim_worker", workerMain);

	if (ctx.getCurrentBinName() == "valsim") {
		if (ctx.containsArg("--show-binaries")) {
			for (const auto &key: ctx.binaries)
				qDebug("%s", key.first.data());
			return 0;
		}
		if (ctx.containsArg("--print-linking-commands")) {
			for (const auto &key: ctx.binaries)
				qDebug("ln -s valsim %s", key.first.data());
			return 0;
		}
		if (ctx.containsArg("--file")) {
			return ctx.runBinary(ctx.binname.toStdString());
		}
		qDebug("usage: valsim [--show-binaries] [--print-linking-commands]");
		return 0;
	}

	return ctx.runBinary(ctx.binname.toStdString());

	//genvideo();

	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " <video_file>" << endl;
		return -1;
	}

	// Open the video file
	VideoCapture cap(argv[1]);
	if (!cap.isOpened())
	{
		cout << "Error opening video file" << endl;
		return -1;
	}

	// Read the first frame
	Mat prev_frame;
	cap >> prev_frame;
	if (prev_frame.empty())
	{
		cout << "Error reading first frame" << endl;
		return -1;
	}
	cvtColor(prev_frame, prev_frame, COLOR_BGR2GRAY);

	// Threshold value for non-zero pixel count
	const int THRESHOLD_VALUE = 30;
	int count = 0;

	int totalFrames = 0;
	int countedFrames = 0;
	int64_t elapsedTotal = 0;

	// Loop through the frames
	Mat curr_frame;
	Mat diff_frame;
	while (cap.read(curr_frame))
	{
		totalFrames++;
		cvtColor(curr_frame, curr_frame, COLOR_BGR2GRAY);

		QElapsedTimer t; t.start();
		// Calculate absolute difference between current frame and previous frame
		absdiff(curr_frame, prev_frame, diff_frame);

		// Threshold the difference frame
		threshold(diff_frame, diff_frame, THRESHOLD_VALUE, 255, THRESH_BINARY);

		// Count non-zero pixels in the thresholded frame
		int non_zero_pixels = countNonZero(diff_frame);
		if (non_zero_pixels > 10000)
			countedFrames++;
		//count += non_zero_pixels;
		elapsedTotal += t.nsecsElapsed();

		// Display the difference frame
		//imshow("Difference Frame", diff_frame);

		// Update previous frame
		prev_frame = curr_frame;

		// Exit if ESC key is pressed
		//if (waitKey(1) == 27)
			//break;
	}

	// Release the video file
	cap.release();

	qDebug() << countedFrames << totalFrames << elapsedTotal << elapsedTotal / totalFrames;
	return 0;
}
