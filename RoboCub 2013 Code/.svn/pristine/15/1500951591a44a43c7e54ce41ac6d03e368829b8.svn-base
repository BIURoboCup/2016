#include "Localization.h"

Localization* Localization::_uniqueInstance = 0;

Localization::Localization()
{
	_uniqueInstance = this;

#ifdef LOCALIZATION_DEBUG
	cout << "Localization::Localization()" << endl;
#endif

	m_algorithm = new Algorithm(this);
	m_robotLocation = m_algorithm->getLocation();
	m_ballHandler = new BallHandler(this);
	m_ballLocation = m_ballHandler->getBallLocationByRef();
	m_GyroHandler = new GyroHandler(this);
#ifdef LOCALIZATION_DEPRECATED
	m_poleHandler = new PoleHandler(this); //TODO DEPRECATED
#endif
	m_poiHandler = new PoiHandler(this);
	m_stepHandler = new StepHandler(this);
}

Localization::~Localization()
{
	delete m_algorithm;
	delete m_ballHandler;
	delete m_GyroHandler;
#ifdef LOCALIZATION_DEPRECATED
	delete m_poleHandler; //TODO DEPRECATED
#endif
	delete m_poiHandler;
	delete m_stepHandler;
}

Localization* Localization::GetInstance()
{
	if (_uniqueInstance == 0)
	{
		new Localization();
	}
	return _uniqueInstance;
}

/*
 * This is the main Localization function
 * after data is ready from vision:
 * 1) handle step diff 		--> move all particles accordingly
 * 2) handle pole pictures 	--> update particles and make algorithm iteration
 * 3) handle ball picture
 *
 * TODO try to add gyro handler again. it was removed because we saw that
 * sometimes it gives really big noise. this could be fixed, and may help a lot!
 * see old implementation below in the old function InternalThreadFunc()
 */
void Localization::process() {
	Diff fromStepDiff;
	m_prevTime = m_currTime;
	m_currTime.update();
	m_usPassed = m_currTime - m_prevTime; // all the sub classes have access to this variables

	//reset localization when robot is took out of field
	if (WalkingAndActions::m_isMiddleButtonPressedLocalization == true)
	{
		ReEnterFiled();
		WalkingAndActions::m_isMiddleButtonPressedLocalization = false;
		return;
	}

	//get step diff (when robot walks)
	fromStepDiff = m_stepHandler->getDiffFromStep();
	if (fromStepDiff.x != 0 || fromStepDiff.y != 0 || fromStepDiff.angle != 0) //robot moved
	{
		m_algorithm->ParticleFilter(fromStepDiff);
#ifdef LOCALIZATION_DEPRECATED
		m_poleHandler->setHasMoved(true); //TODO DEPRECATED
#endif
		m_poiHandler->SetHasMoved(true);
	}
	else
	{
#ifdef LOCALIZATION_DEPRECATED
		m_poleHandler->setHasMoved(false); //TODO DEPRECATED
#endif
		m_poiHandler->SetHasMoved(false);
	}

	//handler pole pictures
	//TODO DEPRECATED
#ifdef LOCALIZATION_DEPRECATED
	if (m_poleHandler->isNewData()) //returns true if accepted new picture
	{
#ifdef LOCALIZATION_DEBUG
		cout << "AFTER if (m_poleHandler->isNewData())" << endl;
#endif
		m_poleHandler->sendDataToAlgo();
	}
#endif

	if(m_poiHandler->IsNewData())
	{
#ifdef LOCALIZATION_DEBUG
		cout << "AFTER if (m_poiHandler->isNewData())" << endl;
#endif
		m_poiHandler->SendDataToAlgo();
	}

	//ball handling
	m_ballHandler->calcBallData();

	//TODO need to check this and optimize
	if (!m_algorithm->amIOut()) {
		m_lastTimeVisionUpdate = m_currTime;
	}

#ifdef LOCALIZATION_PRINT_FIELD
	printField();
#endif

}

bool Localization::amIOut() {
	if (m_currTime - m_lastTimeVisionUpdate > 5 * 1000 * 1000)
		return true;
	return false;
}
/*
 * this is the old main localization thread.
 * now it is invoked from Vision in the function process()
 void Localization::InternalThreadFunc() {
 AngleDiff fromGyroAngleDiff;
 Diff fromStepDiff;
 Diff mergedDiff;
 m_running=true; //set running to false to stop thread.

 while(m_running)
 {

 m_prevTime=m_currTime;
 usleep(50000);

 m_currTime.update();
 m_usPassed=m_currTime-m_prevTime; // all the sub classes have access to this variables

 //ball handling
 m_ballHandler->calcBallData();

 //gyro and step handling
 /////////////////////////////////////////////////////////////////////////////////////

 fromStepDiff=m_stepHandler->getDiffFromStep();
 //TODO i found that the gyro can suddenly give really big noise!!!
 //		 * if(MotionStatus::FALLEN == STANDUP)	{
 //			fromGyroAngleDiff = m_GyroHandler->CheckGyro();
 //			mergedDiff=mergeDiffs(fromStepDiff,fromGyroAngleDiff);
 //		} else {
 //			mergedDiff=fromStepDiff;
 //		}

 mergedDiff=fromStepDiff;
 if (mergedDiff.x !=0 || mergedDiff.y != 0 || mergedDiff.angle !=0 ) //robot moved
 {
 m_algorithm->ParticleFilter(mergedDiff);
 }

 //poles handling
 if (m_poleHandler->isNewData()) //returns true if accepted new picture
 {
 m_poleHandler->sendDataToAlgo();
 }
 }
 }

 */

/*
 * This function was supposed to merge a step diff from the gyros and motion, but is unused
 * because the gyro isn't stable
 */
Diff Localization::mergeDiffs(const Diff& fromStep, const AngleDiff& fromGyro)
{
#ifdef LOCALIZATION_DEBUG
	cout << "Localization::mergeDiffs()" << endl;
#endif
	Diff merged = fromStep;
	merged.angle = (fromStep.angle + fromGyro) / 2;
	return merged;
}

//called by vision:
//Localization class is also proxy for small sub classes
void Localization::setBall(Ball& ball) {
	m_ballHandler->setBall(ball);
}
void Localization::setBall(bool isBallInSight) {
	m_ballHandler->setBall(isBallInSight);
}

//TODO DEPRECATED FUNCTION
#ifdef LOCALIZATION_DEPRECATED
void Localization::setPoles(Pole & pole) {
	m_poleHandler->setPoles(pole);
}
//TODO DEPRECATED FUNCTION
void Localization::setPoles(Pole & left, Pole & right) {
	m_poleHandler->setPoles(left, right);
}
//TODO DEPRECATED FUNCTION
void Localization::setPoles(bool detected) {
	m_poleHandler->setPoles(detected);
}
#endif

void Localization::SetPois(list<PoiData>& poiList)
{
#ifdef LOCALIZATION_DEBUG
	cout << "[Localization::SetPois] Received " << poiList.size() << " new POIs" << endl;
#endif
	m_poiHandler->SetPois(poiList);
}

/*
 * Called by communication when a goal is chosen (beginning and half-time)
 * the color is the color of the goal we defend!!!
 */
void Localization::SetDirection(uint8 color) {
	m_poleHandler->SetDirection(color);
	m_algorithm->flipField();
}

/*
 * for debugging in real time
 * print the field in stdout
 * its not all correct, some bugs, but it doesn't really matter
 * TODO make a better way to see the robots locations in real time by making another program,
 * which will be run on your own computer (not on Darwin) and will listen to all the data the robots
 * send on WIFI, the draw on nice GUI the field as each robot thinks it is, and also a combined view
 * for all the robots. (a tab for each robot, and also a tab for combined data)
 * its not hard and it will be very helpful and also it doesn't take more resource from the robots,
 * because they already send all their data in broadcast.
 * and writing to stdout is heavy!
 */
void Localization::printField() {

	for (int y = 31; y >= -1; --y) {
		for (int x = -22; x < 22; ++x) {
			if (int(m_robotLocation->x) / 10 == x
					&& int(m_robotLocation->y) / 20 == y) {
				//if its the robot location
				if (fabs(m_robotLocation->angle) > 157) {
					cout << '|';
				} else if (fabs(m_robotLocation->angle) > 112) {
					cout << '\\';
				} else if (fabs(m_robotLocation->angle) > 67) {
					cout << '-';
				} else if (fabs(m_robotLocation->angle) > 22) {
					cout << '/';
				} else {
					cout << '|';
				}
			} else if (int(m_ballLocation->x) / 10 == x
					&& int(m_ballLocation->y) / 20 == y) {
				//if its the ball location
				cout << 'O';
			} else if (y == 0 || y == 30) {
				//its the field ver line
				if (abs(x) <= 4) {
					cout << '^'; //its a goal
				} else if (abs(x) <= 20) {
					cout << '-'; //its a line field
				} else {
					cout << ' ';
				}
			} else if (y == 15 && (abs(x) < 20)) {
				cout << "-";
			} else if (abs(x) == 20 && (y >= 0 && y <= 30)) {
				cout << '|';
			} else {
				cout << ' ';
			}
		}
		cout << endl;
	}
	cout << '[' << m_robotLocation->x << ',' << m_robotLocation->y << ','
			<< m_robotLocation->angle << ']' << endl << endl;
}

/*
 * update location by special move
 * 'l' for step left (goalie's steps)
 * 'r' for step right
 * 'L' for jump left
 * 'R' for jump right
 */
void Localization::setStepAside(char direction) {
	Diff diff;
	diff.angle = diff.y = 0;
	diff.time.update();

	switch (direction) {
	case 'r':
		diff.x = 8; //we measured it. if the move is changed, this need to be updated.
		break;
	case 'l':
		diff.x = -8; //we measured it. if the move is changed, this need to be updated.
		break;

	case 'L': //jump left
		diff.x = -10; //we measured it. if the move is changed, this need to be updated.
		diff.y = -15;
		diff.angle = 120;
		break;
	case 'R': //jump right. notice that jumps aren't symmetric.
		diff.x = 0; //we measured it. if the move is changed, this need to be updated.
		diff.y = -15;
		diff.angle = -90;
		break;

	default:
		cout << "void Localization::setStepAside(char direction): "
				<< "the only good options are 'r','l', 'R' or 'L'\n"
				<< "We got " << direction << endl;
		break;
	}

	m_algorithm->ParticleFilter(diff);
}
