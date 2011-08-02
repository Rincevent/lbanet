#include "client.h"
#include "GraphicsWindowQt"
#include "EventsQueue.h"
#include "ClientExtendedEvents.h"

/***********************************************************
constructor
***********************************************************/
Client::Client(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags), _osgwindow(NULL), _Playingvid(false)
{
	ui.setupUi(this);

	connect(ui.videoPlayer, SIGNAL(finished()) , this, SLOT(videofinished()));	
}

/***********************************************************
destructor
***********************************************************/
Client::~Client()
{

}


/***********************************************************
set osg windows
***********************************************************/
void Client::SetOsgWindow(GraphicsWindowQt *wind)
{
	_osgwindow = wind;
	ui.verticalLayout->addWidget(_osgwindow->getGraphWidget());

	HideVideo();
}


/***********************************************************
override close event
***********************************************************/
void Client::closeEvent(QCloseEvent* event)
{
	EventsQueue::getReceiverQueue()->AddEvent(new QuitGameEvent());
}



/***********************************************************
play video
***********************************************************/
void Client::PlayVideo(const std::string & filename)
{
	ui.videoPlayer->show();

	if(_osgwindow)
		_osgwindow->getGraphWidget()->hide();


	Phonon::MediaSource ms(filename.c_str());
	ui.videoPlayer->play(ms);

	_Playingvid = true;
}

/***********************************************************
hide video
***********************************************************/
void Client::HideVideo()
{
	if(_osgwindow)
		_osgwindow->getGraphWidget()->show();

	ui.videoPlayer->hide();

	_Playingvid = false;
}


/***********************************************************
hide video
***********************************************************/
void Client::videofinished()
{
	EventsQueue::getReceiverQueue()->AddEvent(new VideoFinishedEvent());

	_Playingvid = false;
}


/***********************************************************
override keyPressEvent
***********************************************************/
void Client::keyPressEvent (QKeyEvent * event)
{
	if(_Playingvid)
	{
		if(event->key()== Qt::Key_Escape)
		{
			ui.videoPlayer->stop();	
			videofinished();
			return;
		}
	}

	QWidget::keyPressEvent(event);
}