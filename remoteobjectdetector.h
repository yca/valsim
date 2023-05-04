#ifndef REMOTEOBJECTDETECTOR_H
#define REMOTEOBJECTDETECTOR_H

#include "objectdetectorinterface.h"

namespace valsim {

class RemoteObjectDetector : public valsim::ObjectDetectorInterface
{
public:
	RemoteObjectDetector(const std::string &ep);

	int detectFrame(const cv::Mat &frame, std::vector<Detection> &dets);

protected:
	std::string remotedev;
};

}

#endif // REMOTEOBJECTDETECTOR_H
