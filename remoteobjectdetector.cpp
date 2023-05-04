#include "remoteobjectdetector.h"

#include <thread>
#include <rpc/client.h>
#include <bpf_tools/distproxyserver.h>

#include <QDebug>

using namespace valsim;
using namespace std::chrono_literals;

RemoteObjectDetector::RemoteObjectDetector(const std::string &ep)
	: remotedev{ep}
{

}

int RemoteObjectDetector::detectFrame(const cv::Mat &frame, std::vector<Detection> &dets)
{
	std::string program = "nvidia-smi";
	std::vector<std::string> programArgs;
	programArgs.push_back("-i");
	programArgs.push_back("0,1,2,3");
	DistributeRequest req;
	int timeout = 10000;
	req.job.jobType = REMOTE_JOB_RUN_PROCESS;
	req.job.jobRun.program = program;
	req.job.jobRun.arguments = programArgs;
	req.includeProfileData = false;
	req.waitTimeout = timeout;
	rpc::client c(remotedev, 40001);
	auto res = c.call("distribute", req).as<DistributeResponse>();
	while (res.error == -ENOENT) {
		/* don't have enough workers, let's retry in a while */
		std::this_thread::sleep_for(1ms);
		res = c.call("distribute", req).as<DistributeResponse>();
	}
	qDebug("request completed with %d", res.error);
	qDebug() << res.jobResp.jobRun.out.data();
	qDebug() << res.jobResp.jobRun.err.data();
	return res.error;
}
