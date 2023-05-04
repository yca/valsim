#ifndef OBJECTDETECTORINTERFACE_H
#define OBJECTDETECTORINTERFACE_H

#include <iostream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

namespace valsim {

struct BoundingBox {
	float x, y, w, h;
	float score;
	int categoryId;
};

struct Detection {
	std::string imageId;
	BoundingBox bbox;
};

struct Category {
	int id;
	std::string name;
};

struct CocoDataset {
	std::vector<Detection> detections;
	std::vector<Category> categories;
};

class ObjectDetectorInterface
{
public:
	virtual int detectFrame(const cv::Mat &frame, std::vector<Detection> &dets) = 0;
	virtual int detectFrame(const std::vector<cv::Mat> &frames,
							std::vector<std::vector<Detection>> &dets);
	virtual int detectFile(const std::string &filename, std::vector<Detection> &dets);
	virtual int detectFile(const std::vector<std::string> &filenames,
						   std::vector<std::vector<Detection>> &dets);
	virtual int detectFolder(const std::string &folderName,
							 std::vector<std::vector<Detection>> &dets);
};

}

#endif // OBJECTDETECTORINTERFACE_H
