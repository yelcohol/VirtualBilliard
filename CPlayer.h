#pragma once
class CPlayer {
private:
	bool first_player;
	int score;
public:
	CPlayer(int _score);
public:
	int printscore();
	void getscore();
	void losescore();
	void setscore(int score);
	void set_first_player(bool fp);
	bool get_first_player();
	void turn();

};
