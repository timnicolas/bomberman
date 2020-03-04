#include "AScene.hpp"

AScene::AScene() {
}

AScene::AScene(AScene const & src) {
	*this = src;
}

AScene::~AScene() {
}

AScene & AScene::operator=(AScene const & rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}
