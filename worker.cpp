#include <opencv2/opencv.hpp>
#include <commonpp/mainappcontext.h>

#include <rpc/client.h>
#include <bpf_tools/distproxyserver.h>

#include <QUuid>
#include <QDebug>
#include <QProcess>

using namespace cv;


static WorkerJobResponse runProcess(const WorkerJobRequest &job)
{
	qDebug() << "running" << job.jobRun.program.data();
	WorkerJobResponse jobResp;
	QProcess p;
	p.setProgram(job.jobRun.program.data());
	QStringList qargs;
	for (int i = 0; i < job.jobRun.arguments.size(); i++)
		qargs << job.jobRun.arguments[i].data();
	p.setArguments(qargs);
	p.start();
	p.waitForStarted(-1);
	p.waitForFinished(-1);
	{
		const auto &ba = p.readAllStandardOutput();
		jobResp.jobRun.out.append(QString::fromUtf8(ba).trimmed().toStdString());
	}
	{
		const auto &ba = p.readAllStandardError();
		jobResp.jobRun.err.append(QString::fromUtf8(ba).trimmed().toStdString());
	}
	jobResp.jobRun.exitCode = p.exitCode();

	return jobResp;
}

int workerMain(MainAppContext &ctx)
{
	const auto &proxyep = ctx.getStringArg("--proxy").toStdString();
	if (proxyep.empty()) {
		qDebug("invalid proxy address");
		return -EINVAL;
	}
	rpc::client c(proxyep, 40001);
	int timeout = 10000;
	if (ctx.containsArg("--wait-timeout"))
		timeout = ctx.getIntArg("--wait-timeout");
	auto myuuid = QUuid::createUuid().toString(QUuid::Id128).toStdString();
	{
		RegisterRequest req;
		req.uuid = myuuid;
		if (c.call("register", req).as<int>()) {
			qDebug("error registering ourselves to the proxy");
			return -EINVAL;
		}
	}

	while (1) {
		JobRequest req;
		req.timeout = timeout;
		req.uuid = myuuid;
		const auto &res = c.call("request", req).as<JobResponse>();
		if (!res.error) {
			CompleteRequest req;
			req.workerid = myuuid;
			qDebug("we have a new job to do, type=%d", res.job.jobType);
			req.jobResp = runProcess(res.job);
			req.jobResp.jobType = res.job.jobType;
			const auto &res = c.call("complete", req).as<CompleteResponse>();
		} else {
			qDebug("job request error %d", res.error);
		}
	}

	return 0;
}

