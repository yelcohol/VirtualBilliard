#include "CPlayer.h"

CPlayer::CPlayer(int _score) {
	score = _score;
}
int CPlayer::printscore() {
	return this->score;
}
void CPlayer::getscore() {
	this->score -= 10;
}
void CPlayer::losescore() {
	this->score += 10;
}
void CPlayer::setscore(int score) {
	this->score = score;
}
void CPlayer::set_first_player(bool fp) {
	this->first_player = fp;
}
bool CPlayer::get_first_player() {
	return this->first_player;
}
void CPlayer::turn() {
	this->first_player = !(this->first_player);
}


