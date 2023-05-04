#include "objectdetectorinterface.h"

#include <filesystem>

namespace fs = std::filesystem;

static void listFiles(const std::string& path, std::vector<std::string> files)
{
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_directory())
			listFiles(entry.path().string(), files); // recursively list files in the subdirectory
		else if (entry.is_regular_file())
			files.push_back(entry.path().string());
	}
}

int valsim::ObjectDetectorInterface::detectFrame(const std::vector<cv::Mat> &frames,
												 std::vector<std::vector<Detection> > &dets)
{
	for (const auto &frame: frames) {
		std::vector<Detection> det;
		int err = detectFrame(frame, det);
		if (err)
			return err;
		dets.push_back(det);
	}
	return 0;
}

int valsim::ObjectDetectorInterface::detectFile(const std::string &filename, std::vector<Detection> &dets)
{
	return detectFrame(cv::imread(filename), dets);
}

int valsim::ObjectDetectorInterface::detectFile(const std::vector<std::string> &filenames, std::vector<std::vector<Detection> > &dets)
{
	for (const auto &file: filenames) {
		std::vector<Detection> det;
		int err = detectFile(file, det);
		if (err)
			return err;
		dets.push_back(det);
	}
	return 0;
}

int valsim::ObjectDetectorInterface::detectFolder(const std::string &folderName, std::vector<std::vector<Detection> > &dets)
{
	std::vector<std::string> files;
	listFiles(folderName, files);
	return detectFile(files, dets);
}
