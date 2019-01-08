#pragma once
template<class ClassName>
class Key
{
public:
	ClassName instance;

	void openHandle() {
		instance.openHandle();
	}
	void closeHandle() {
		instance.closeHandle();
	}
	void upAllKey() {
		instance.upAllKey();
	}
	int getKeyState(int keyCode) {
		return instance.getKeyState(keyCode);
	}
	void keyDown(int keyCode) {
		instance.keyDown(keyCode);
	}
	void keyUp(int keyCode) {
		instance.keyUp(keyCode);
	}
	void doKeyPress(int keyCode, int s = 100) {
		instance.doKeyPress(keyCode, s);
	}
	void mouseClick() {
		instance.mouseClick();
	}
	void mouseDoubleClick(int s = 100) {
		instance.mouseDoubleClick(s);
	}
	void moveMousePos(int x, int y) {
		instance.moveMousePos(x, y);
	}
	void setMousePos(int x, int y) {
		instance.setMousePos(x, y);
	}
	void getMousePos(int * x, int * y) {
		instance.getMousePos(x, y);
	}

};

