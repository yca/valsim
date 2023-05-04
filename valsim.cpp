#include "remoteobjectdetector.h"

#include <opencv2/opencv.hpp>
#include <commonpp/mainappcontext.h>

#include <QFile>
#include <QDebug>

using namespace cv;
using namespace valsim;

int valsimMain(MainAppContext &ctx)
{
	const auto &filename = ctx.getStringArg("--file").toStdString();
	if (!QFile::exists(filename.data())) {
		qDebug("input file does not exist");
		return -ENOENT;
	}
	const auto &proxyep = ctx.getStringArg("--proxy").toStdString();
	if (proxyep.empty()) {
		qDebug("invalid proxy address");
		return -EINVAL;
	}
	// Open the video file
	VideoCapture cap(filename);
	if (!cap.isOpened())
	{
		qDebug() << "Error opening video file";
		return -1;
	}

	// Read the first frame
	Mat prev_frame;
	cap >> prev_frame;
	if (prev_frame.empty())
	{
		qDebug() << "Error reading first frame";
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

		//valsim::RemoteObjectDetector det("10.5.20.40");
		valsim::RemoteObjectDetector det(proxyep);
		std::vector<Detection> dets;
		det.detectFrame(curr_frame, dets);
	}

	// Release the video file
	cap.release();

	return 0;
}
