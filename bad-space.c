static char *version = "vv space invaders 3M";

/*T. Barake, Nov-2021			recompiled under VS*/
/*T. Barake, Dec-82				turbo C version    */

#include    <stdio.h>
#include    <ctype.h>
#include    <conio.h>
#include    <stdlib.h>
#include	<string.h>
//#define _WIN32_WINNT 0x0500
#include	<windows.h> 

/*macros*/
#define        gtch()  (oldc)

//#define        max(a,b)  ((a)>(b)?(a):(b))
/*constants*/

/*#define        HYPERION        1 */           /*cond compilation flag*/
#define _NOCURSOR		0
#define _NORMALCURSOR	1

void draw_score();
void idle_screen();
void move_gun();
void gun_missile();
void gun_noise();
void move_missile();
void move_invaders();
void refresh_gun();
int  play_again();
void move_down();
void generate_new_missile(int i);
void displaystr(char *str);
void scrout(int page, int x, int y, char *str, int attr);
void init_screen();

//dummies
void textattr(int attr) {}
void _setcursortype(int i) { }
void clrscr() {};



void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void delay(int i) { Sleep(i); }

void window(int a, int b, int c, int d) {

	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, (c - a) * 9, (d - b) * 16, TRUE); // width, height
}



#ifdef HYPERION

#define        ATTR_N          0x00
#define        ATTR_B          0x04
#define        ATTR_D          0xC0
#define        ATTR_R          0x02

#else      /*for IBM*/

#define        ATTR_N          0x107
#define        ATTR_B          0x180
#define        ATTR_D          0x108
#define        ATTR_R          0x170

#endif

#define        MAX_LIN         25
#define        MAX_COL         80
#define        L_EDGE          1
#define        R_EDGE          (MAX_COL-1)

#define        GUN_LIN         21
#define        SCORE_LIN       24
#define        BLOCK_LIN       18

#define        INV_ROWS        5
#define        INV_COLS        10
#define        MAX_MISSILES    3

#define        INV_INCR        1
#define        EMPTY           -1
#define        LEFT            0
#define        RIGHT           1

#define        GUN_CHAR        (0x1F & 'S')     /*ctrl S */
#define        MIS_CHAR        (0x1F & 'Y')     /*ctrl Y */
#define        SHOT_CHAR       (0x1F & 'E')     /*ctrl E */
#define        BLOCK_CHAR      (0x1F & 'V')     /*ctrl V */
#define        EXP_CHAR        '*'

#define        SHOOT           ' '
#define        MOVE_LEFT       '['
#define        MOVE_RIGHT      ']'

//#ifdef HYPERION
#define        SPKR_PORT       0x48            /*hyperion speaker port*/
#define        INV_DUR         3               /*invader movement sounds*/
#define        INV_FREQ        20
#define        SHOT_DUR        60              /*sound of shot*/
#define        EXP_FREQ        400             /*gun explosion*/
//#endif

#define        VALUEA          10
#define        VALUEB          25
#define        VALUEC          50
#define        FREE_GUN       1200
#define        DELAY           2               /*missile speed*/
#define        DOWN_DELAY      1
#define        GUN_DELAY       10              /*no of main cycles */
#define        SEED            13              /*rnd seed*/
#define        GOOD_SCORE      2*FREE_GUN      /*missiles speed up after this*/


/*types*/

struct invader
{
	int col[2];
};

struct invader_lin
{
	int top_lin;
	int bot_lin;
	int value;
	int no_inv;                                          /*how many left*/
	char *neg_image[2];                                  /*invader image*/
	char *tpos_image[2];
	char *bpos_image[2];
	struct invader node[INV_COLS];
};

struct projectile
{
	int lin;                                             /*can be EMPTY*/
	int col;
};

/*variables*/

char fn[] = "space.exe";                 /*filename*/
char decoy[] = "T. Barake 1983.";
char inva1a[] = "-o|o-";
char inva1b[] = "-:|:-";
char inva2a[] = "ooooo";
int  inval = 5;
char nega[] = "     ";

char invb1a[] = "OooO";
char invb1b[] = "-oo-";
char invb2a[] = "mmmm";
char invb2b[] = "nnnn";
int  invbl = 4;
char negb[] = "    ";

char invc1a[] = "=o=";
char invc1b[] = "\\o/";                                  /*watch out for esc*/
char invc2a[] = "ooo";
char invc2b[] = ":::";
int  invcl = 3;
char negc[] = "   ";

char screen[MAX_LIN][MAX_COL + 1];                      /*screen image*/

char *gun_image[3] =
{ "",
 "",
 "" };

char *ngun_image[3] =
{ " ",
 "   ",
 "     " };

char block_image[3][11] =
{ "  ",
 "",
 "      " };

char mysta[] = "<[][]>";
char mystb[] = "<][][>";
char mystscore[] = "<    >";
char nmyst[] = "      ";

/*#ifdef HYPERION*/
int sound = FALSE;
int freq = 0;
/*#endif*/

char oldc = '\0';                                        /*used in gtch*/
char ooc;
char blank[MAX_COL + 1];
char code = ' ';

int cur_row;
int direction;                                         /*invader direction*/
int image;                                             /*current image index*/
int no_of_invaders;

int signature[] = { 0x35,0x4f,0x41,0x23,0x00,0x13,0x00,0x0a,0x04,0x41,
/* T    .         B    a    r    a    k    e         <> dummy */
0x50,0x58,0x59,0x52,0x4f,0xff,0x1a };
/* 1    9    8    3    .  <EOS>  dummy */
int score = 0;
int old_score = -1;
int old_guns = 0;
int hi_score = 0;
int offset = 0;
unsigned int play;

int no_of_guns;
int gun_col;
int dead;
int mdelay = DELAY;
int mmdelay = DELAY;
int gun_delay;
int down_count = 0;
int rnd = SEED;

struct projectile gun_shot = { EMPTY,EMPTY };
struct projectile missile[MAX_MISSILES];               /*invader missiles*/
struct invader_lin alien[INV_ROWS];                    /*all invaders*/

/**/
inline void inv_missiles() { mdelay = ((mdelay) ? (mdelay - 1) : (move_missile(), mmdelay)); }
inline int  isblank(c) { return ((c) == ' ') || ((c) == '\0'); }
inline void gtc() { if (_kbhit()) oldc = (oldc == (ooc = getch()) ? '\0' : ooc); else oldc = '\0'; }
inline int mchk() { return oldc == '\0' ? 0 : 1; }
inline void gun_sound() { sound && freq > 0 ? gun_noise() : 0; }


int main(int argc, char *argv[])
{
	int i;

	window(1, 1, 80, 25);
	clrscr();
	_setcursortype(_NOCURSOR);
#ifdef HYPERION
	mode(0x82);                                   /*set hyperion mode*/
	if (argc >= 2) sound = NO;                    /*optional sound effects*/
#endif

	for (i = 0; i < MAX_COL; blank[i++] = ' ')
		;
	blank[MAX_COL] = '\0';

	code = 0x61;                                    /*must be 'a'*/
	displaystr("IN:VADERS 1.01 ");
	//for (play = 0; play < 0xFFFF; play++);
	delay(1000);

	/*
	for (play=0;signature[play]!=0xff;play++)
	 if ( (char)(signature[play]^code) != decoy[play])
	  {
	  int fp;
	  if ((fp=creat(fn,0x1001)) != -1 )
	   {
	   write(fp,screen,MAX_LIN*(MAX_COL+1));
	   close(fp);
	   }
	  printf("\nbye bye \"%s\"\n",argv[0]);
	  exit(0);
	  }
	*/

	do
	{
		idle_screen();
		while (no_of_guns > 0)
		{
			draw_score();
			gun_sound();
			gtc();
			if (gun_delay == 0)
				move_gun();
			gun_sound();
			gun_missile();
			gun_sound();
			inv_missiles();         /*macro*/
			gun_sound();
			gtc();
			gun_sound();
			gun_missile();
			gun_sound();
			gtc();
			if (gun_delay == 0)
				move_gun();
			gun_sound();
			move_invaders();
			if (dead || gun_delay > 0)
				refresh_gun();
			gun_sound();
			delay(70);
		}
		play = play_again();
	} while (play);
	_setcursortype(_NORMALCURSOR);
	return 0;
}/*main*/

void gun_noise()
{
#ifdef HYPERION
	int i, j;
	for (i = 0; i < 3; i++)
	{
		outp(SPKR_PORT, 0xFF);
		for (j = 0; j < freq; j++);
		outp(SPKR_PORT, 0x00);
		freq--;
	}
#else
	for (int i = 0; i < 3; i++)  {
		for (int j = 0; j < freq; j++)
			;
		Beep(freq, 10);
		freq--;
	}
#endif
}
/**/

void move_invaders()
/*moves the current row of space invaders one INCR in current direction	*/
/*is responsible for changing direction when the extreme invader hits a	*/
/*wall. this routine ignores any invader line if the line number is		*/
/*EMPTY. it is responsible for updating the current line number		    */
{
	int i;
	int incr, i_incr;
	int top_lin, bot_lin;
	struct invader *ptr;
	struct invader_lin *aptr;
	char *neg0, *pos0, *neg1, *pos1, c;                   /*invader image*/

#ifdef HYPERION
	if (sound == YES)
	{
		for (i = 0; i < INV_DUR; i++)
		{
			outp(SPKR_PORT, 0xFF);
			for (j = 0; j < INV_FREQ - i; j++);
			outp(SPKR_PORT, 0x00);
		}
	}
#endif

	/*check if invaders have reached the boundary*/
	do {
		if (++cur_row == INV_ROWS)	{
			cur_row = 0;
			image ^= 1;										 /*flip the image */
			int edge, ndir;
			switch (direction) {
			case LEFT:
				edge = L_EDGE;
				ndir = RIGHT;
				break;
			case RIGHT:
				edge = R_EDGE;
				ndir = LEFT;
				break;
			}
			for (i = 0; i < GUN_LIN; i++) {
				if ((c = screen[i][edge]) != MIS_CHAR && !isblank(c) && c != SHOT_CHAR) {
					move_down();
					direction = ndir;     /*change direction*/
					break;
					}
				}
			}
		}
	while (alien[cur_row].no_inv == 0);

	if (dead)					 /*after down movement*/
		return;

	aptr = &alien[cur_row];

	top_lin = aptr->top_lin;
	bot_lin = aptr->bot_lin;

	neg0 = aptr->neg_image[0];
	neg1 = aptr->neg_image[1];
	pos0 = aptr->tpos_image[image];
	pos1 = aptr->bpos_image[image];

	if (direction == LEFT) {
		ptr = &aptr->node[0];
		incr = 1;
		i_incr = -INV_INCR;
	}
	else {
		ptr = &aptr->node[INV_COLS - 1];
		incr = -1;
		i_incr = INV_INCR;
	}

	for (i = 0; i < INV_COLS; i++)	{
		if (ptr->col[LEFT] != EMPTY) {                     /*there is an invader there*/
			scrout(0, top_lin, ptr->col[LEFT], neg0, ATTR_N);   /*move invader*/
			scrout(0, bot_lin, ptr->col[LEFT], neg1, ATTR_N);
			strcpy(&screen[top_lin][ptr->col[LEFT]], neg0);  /*fixup screen image*/
			strcpy(&screen[bot_lin][ptr->col[LEFT]], neg1);

			ptr->col[LEFT] += i_incr;
			ptr->col[RIGHT] += i_incr;

			scrout(0, top_lin, ptr->col[LEFT], pos0, ATTR_N);
			scrout(0, bot_lin, ptr->col[LEFT], pos1, ATTR_N);
			strcpy(&screen[top_lin][ptr->col[LEFT]], pos0);
			strcpy(&screen[bot_lin][ptr->col[LEFT]], pos1);
		}
		ptr += incr;
	}


}

/**/

/*  routine responsible for:                                   */
/* -generating a constant barrage of invader missiles          */
/* -moving invader         missiles and checking for kills     */
void move_missile()
{
	int  i, *lin, *col;
	char c, str[2];

	str[0] = MIS_CHAR;
	str[1] = '\0';

	/*process invader missiles*/

	for (i = 0; i < MAX_MISSILES; i++) 	{
		lin = &missile[i].lin;
		col = &missile[i].col;

		if (screen[*lin][*col] == EXP_CHAR) {    /*a splat to be deleted*/
			screen[*lin][*col] = ' ';
			scrout(0, *lin, *col, " ", ATTR_N);
			*lin = EMPTY;
		}

		if (*lin == EMPTY)	{
			generate_new_missile(i);
			continue;
		}

		/*delete old missile image, generate new missile coords*/

		screen[*lin][*col] = ' ';
		scrout(0, *lin, *col, " ", ATTR_N);
		(*lin) += 1;
		/*beyond block lin*/
		if (*lin > GUN_LIN + 3)	{
			*lin = EMPTY;                                     /*off limits*/
			continue;
		}

		if (isblank(screen[*lin][*col]))  {                   /*no hit*/
			screen[*lin][*col] = MIS_CHAR;
			scrout(0, *lin, *col, str, ATTR_N);
			continue;
		}
		else
			scrout(0, *lin, *col, " ", ATTR_N);               /*default*/

		if (*lin >= BLOCK_LIN) {                              /*beyond gun*/
			if (*lin >= GUN_LIN) {                            /*beyond last lin*/
				dead = TRUE;
				no_of_guns--;
				screen[*lin][*col] = ' ';
				*lin = EMPTY;
				continue;
			}
			else {
				screen[*lin][*col] = ' ';
				*lin = EMPTY;
				continue;								// block is hit
			}
		}

		if (screen[*lin][*col] == SHOT_CHAR)	{		// missile to missile
			char str2[2];
			str2[0] = EXP_CHAR;
			str2[1] = '\0';

			screen[*lin][*col] = EXP_CHAR;
			scrout(0, *lin, *col, str2, ATTR_N);
			gun_shot.lin = EMPTY;
			gun_shot.col = EMPTY;
			continue;
		}
	}
}

/**/


/* -move or generate the gun missile*/
/* -keeping track of dead invaders and updating data struct    */
/* -missiles can also intercept invader missiles               */
/* -redrawing a new screen if all invaders dead                */
void gun_missile()
{
	int i, j;
	char c, str[2];
	struct invader *p;

	str[0] = SHOT_CHAR;
	str[1] = '\0';

	if (gun_shot.lin == EMPTY) {
		if (mchk()) {
			if (SHOOT == (c = gtch())) {
				int i, j;
				gun_shot.lin = GUN_LIN - 1;
				gun_shot.col = gun_col;

				//#ifdef HYPERION
				if (sound)
					freq = SHOT_DUR;
				//#endif

				goto first_time;
			}
			else
				return;
		}
		else
			return;                                              /*do nothing*/
	}

		  /*always erase old gun shot*/

	screen[gun_shot.lin + 1][gun_shot.col] = ' ';
	scrout(0, gun_shot.lin + 1, gun_shot.col, " ", ATTR_N);

	if (gun_shot.lin <= 0)  {                                 /*boundary*/
		gun_shot.lin = EMPTY;
		return;
	}

first_time:

	if (isblank(screen[gun_shot.lin][gun_shot.col])) {     /*check for no hits*/
		screen[gun_shot.lin][gun_shot.col] = SHOT_CHAR;
		scrout(0, gun_shot.lin, gun_shot.col, str, ATTR_N);
		gun_shot.lin--;                                       /*rewrite shot*/
		return;
	}

	/*something has been hit at next position*/

	if (gun_shot.lin >= BLOCK_LIN                              /*a block*/
		&&screen[gun_shot.lin][gun_shot.col] == BLOCK_CHAR) { 
		screen[gun_shot.lin][gun_shot.col] = ' ';
		scrout(0, gun_shot.lin, gun_shot.col, " ", ATTR_N);
		gun_shot.lin = EMPTY;                                  /*kill missile*/
		return;
	}
	/*could be missile*/
	if (screen[gun_shot.lin][gun_shot.col] == MIS_CHAR)	{
		char str2[2];
		str2[0] = EXP_CHAR;
		str2[1] = '\0';

		for (i = 0; i < MAX_MISSILES && (missile[i].col != gun_shot.col ||
			missile[i].lin != gun_shot.lin); i++)
			;
		screen[gun_shot.lin][gun_shot.col] = EXP_CHAR;
		scrout(0, gun_shot.lin, gun_shot.col, str2, ATTR_N);
		gun_shot.lin = EMPTY;
		return;
	}
	/*must be invader*/
	for (i = 0; i < INV_ROWS; i++) {                          /*for all rows*/
		if (alien[i].no_inv > 0 &&                            /*possible hit*/
			(gun_shot.lin == alien[i].bot_lin || gun_shot.lin == alien[i].top_lin)) {
			for (j = 0; j < INV_COLS; j++) {                        /*scan for hit*/
				p = &alien[i].node[j];
				if (p->col[LEFT] != EMPTY&& p->col[LEFT] <= gun_shot.col &&
					gun_shot.col <= p->col[RIGHT]) {  /*a hit*/

					int  *l, *r;

					l = &p->col[LEFT];
					r = &p->col[RIGHT];
					alien[i].no_inv--;
					score += alien[i].value;
					gun_shot.lin = EMPTY;
					strcpy(&screen[alien[i].top_lin][*l], alien[i].neg_image[0]);
					strcpy(&screen[alien[i].bot_lin][*l], alien[i].neg_image[1]);
					scrout(0, alien[i].top_lin, *l, alien[i].neg_image[0], ATTR_N);
					scrout(0, alien[i].bot_lin, *l, alien[i].neg_image[1], ATTR_N);
					*l = EMPTY;
					*r = EMPTY;
					--no_of_invaders ? 0 : (init_screen(), refresh_gun());        /*killed all*/
					return;
				}
			}
		}
	}
}

/**/


void move_gun_util(char *gunImg[])
{
	int i;
	for (i = 0; i < 3; i++) {
		strcpy(&screen[GUN_LIN + i][gun_col - i], ngun_image[i]);
		scrout(0, GUN_LIN + i, gun_col - i, gunImg[i], ATTR_N);
	}
}

/**/
/*scans keyboard for key pressed to check if gun is to be moved*/
/*moves the gun if necessary    */

void move_gun()
{
	int i;
	char c;

	if (mchk()) {
		if ((c = gtch()) == MOVE_LEFT || c == MOVE_RIGHT) {
			int move = FALSE;
			// erase gun first
			switch (c) {
			case MOVE_LEFT:
				if ((gun_col - 2) > L_EDGE) {
					move_gun_util(ngun_image);
					gun_col--;
					move = TRUE;
				}
				break;

			case MOVE_RIGHT:
				if ((gun_col + 2) < R_EDGE)	{
					move_gun_util(ngun_image);
					gun_col++;
					move = TRUE;
				}
				break;
			}
			// redraw gun
			if (move) {
				move_gun_util(gun_image);
			}
		}
	}
}
/**/


/*called to redraw gun if it has been destroyed*/
/*will explode current gun                     */
/*will also reset dead flag                    */

void refresh_gun()
{
	unsigned int j;
	register int i;

	if (dead) {
		dead = FALSE;
		gun_delay = GUN_DELAY;
		for (i = 0; i < 3; i++)                     /*explode current gun if any*/
			scrout(0, GUN_LIN + i, gun_col - i, gun_image[i], ATTR_B | ATTR_R);

#ifdef HYPERION
		if (sound == YES)
			for (i = 0; i < EXP_FREQ; i++)
			{
				int j;
				outp(SPKR_PORT, 0xFF);
				for (j = 0; j < EXP_FREQ - i; j++);
				outp(SPKR_PORT, 0x00);
			}
#else
		delay(1000);
		//for (j = 0; j < 0xFFFF; j++);               /*loop a while*/
#endif

		for (i = 0; i < 3; i++)   {                  /*erase gun*/
			scrout(0, GUN_LIN + i, gun_col - i, ngun_image[i], ATTR_N);
			strcpy(&screen[GUN_LIN + i][gun_col - i], ngun_image[i]);
		}
	}

	/*draw a gun if time is right*/

	--gun_delay;

	if (no_of_guns > 0 && gun_delay == 0) {
		gun_col = L_EDGE + 2;                      /* starting position for gun*/
		for (i = 0; i < 3; i++)    {               /* redraw gun*/
			strcpy(&screen[GUN_LIN + i][gun_col - i], gun_image[i]);
			scrout(0, GUN_LIN + i, gun_col - i, gun_image[i], ATTR_N);
		}
	}

	if (gun_shot.lin != EMPTY&&gun_shot.col != EMPTY) {
		scrout(0, gun_shot.lin + 1, gun_shot.col, " ", ATTR_N);
		screen[gun_shot.lin + 1][gun_shot.col] = ' ';
	}
	gun_shot.col = EMPTY;
	gun_shot.lin = EMPTY;
}

/**/

int play_again()
/*asks user for new game*/
{
	int i;
	char c, str[80];


	for (i = 0; i < MAX_LIN; i++)
		scrout(1, i, 0, blank, ATTR_N);

	/*set_page(1);*/

	sprintf(str, "Your score: %4d", score);
	scrout(1, 12, 0, str, ATTR_D);
	scrout(1, 23, 0, "Play Again ?", ATTR_D | ATTR_B);

	do c = getch(); while (c != 'Y'&&c != 'y'&&c != 'N'&&c != 'n');

	putch(c);

	switch (c)	{
	case 'n':
	case 'N':
		for (i = 0; i < 25; i++)
			scrout(0, i, 0, blank, ATTR_N);
		/* set_page(0);*/
		return FALSE;
		break;

	case 'y':
	case 'Y':
		return TRUE;
		break;
	}
	return  FALSE ;
}

/**/

void init_score()
/*sets up a new game*/
{
	if (score > hi_score)
		hi_score = score;

	score = 0;
	old_score = -1;
	no_of_guns = 3;
	old_guns = 0;
	offset = 0;

	draw_score();
}

/**/

void draw_score()
/*writes score to display*/
{
	int i;
	char lstr[30];
	char str[2];

	str[1] = '\0';
	str[0] = SHOT_CHAR;

	if (score - offset >= FREE_GUN)	{
		no_of_guns++;
		offset += FREE_GUN;
	}

	if (score != old_score || no_of_guns != old_guns)	{
		old_score = score;

		for (i = 0; i < (max(old_guns, no_of_guns)); i++)
			if (i < no_of_guns)
				scrout(0, SCORE_LIN, i, str, ATTR_N);
			else
				scrout(0, SCORE_LIN, i, " ", ATTR_N);

		old_guns = no_of_guns;


		sprintf(lstr, "Points:%4d  High Score:%4d", score, hi_score);
		scrout(0, SCORE_LIN, 48, lstr, ATTR_N);
		if (score > GOOD_SCORE) mmdelay = DELAY - 1;      /*speed up missiles*/

	}
}

/**/

void displaystr(char *str)
/*decodes string signature and prints it*/
{
	unsigned int i;

	for (i = 0; str[i] != 0; i++)
		putch(str[i]);
	for (i = 0; signature[i] != 0xFF; i++)
		putch((char)(signature[i] ^ code));
}

/**/

void idle_screen()
/*new game*/
{
	int i;
	char str[2], str2[60];

	str[1] = '\0';

	for (i = 0; i < MAX_LIN; i++)
		scrout(1, i, 0, blank, ATTR_N);

	scrout(1, 12, 0, "Hyperion Invaders!", ATTR_D);
	scrout(1, 14, 0, "Use \"", ATTR_N);
	str[0] = MOVE_LEFT;
	scrout(1, 14, 5, str, ATTR_D);
	scrout(1, 14, 7, "\" and \"", ATTR_N);
	str[0] = MOVE_RIGHT;
	scrout(1, 14, 13, str, ATTR_D);
	scrout(1, 14, 16, "\" keys to move gun, space bar to fire.", ATTR_N);
	sprintf(str2, "Extra gun after each %d points", FREE_GUN);
	scrout(1, 16, 0, str2, ATTR_N);
	scrout(1, 23, 0, "Hit a Key When Ready ", ATTR_D | ATTR_B);
	/*set_page(1);*/

	putch(getch());

	dead = FALSE;
	play = TRUE;
	no_of_guns = 3;
	mdelay = mmdelay;

	/*set_page(0);*/
	init_screen();
	refresh_gun();
	init_score();

	return;
}

/**/

void generate_new_missile(int i)

/*creates new missile coordinates for specified missile*/
/*bases itself on gun position, invader position in bottom row*/
{
	int l, c, skew, k;
	struct invader *p;
	struct projectile *pr;

	pr = &missile[i];
	rnd = (++rnd) % SEED;
	skew = -1;                                       /*missile skew*/
	k = (rnd & 0x06) >> 1;                            /*take middle bits*/

label0:
	switch (k) {
	case 0:                                       /*shoot from first row, front*/
		for (l = 0; l < INV_ROWS; l++)
			if (alien[l].no_inv > 0) break;              /*find bottom row*/
		for (c = 0; c < INV_COLS; c++) {
			p = &alien[l].node[c];
			if (p->col[LEFT] != EMPTY) break;   /*first from left*/
		}
		pr->lin = alien[l].bot_lin + 1;
		pr->col = p->col[LEFT] + 2;
		break;

	case 1:
		for (l = 0; l < INV_ROWS; l++)
			if (alien[l].no_inv > 0) break;             /*find bottom row*/
		for (c = INV_COLS - 1; c >= 0; c--) {
			p = &alien[l].node[c];
			if (p->col[LEFT] != EMPTY) break; /*first from right*/
		}
		pr->lin = alien[l].bot_lin + 1;
		pr->col = p->col[RIGHT] - 2;
		break;

	case 2:                                      /*go for the gun position*/
		skew = 1;
	case 3:
	{
		char s;
		for (l = GUN_LIN - 1; l >= 0; l--) {                  /*start at gun*/
			s = screen[l][gun_col + skew];
			if (!isblank(s) && (s != BLOCK_CHAR) && (s != SHOT_CHAR) && (s != MIS_CHAR)
				&& (s != EXP_CHAR)) {
				pr->lin = l + 1;
				pr->col = gun_col + skew;
				return;
			}
		}
		k &= 1;                                         /*mask upper bit*/
		goto label0;                                  /*do case 0 or instead*/
	}

	}
}

/**/

void move_down()
{
	int r, c, tl, bl, obl, otl;
	char *n0, *n1, *p0, *p1;
	struct invader_lin *rptr;
	struct invader *iptr;

	// check timing
	if (down_count != 0) {
		down_count--;
		return;
	}
	else
		down_count = DOWN_DELAY;


	// move them down 
	for (r = 0; r < INV_ROWS; r++) {
		if (alien[r].no_inv > 0) {
			rptr = &alien[r];
			otl = rptr->top_lin;
			obl = rptr->bot_lin;
			tl = rptr->top_lin += 1;
			bl = rptr->bot_lin += 1;

			if (bl >= GUN_LIN) {           /*aliens have invaded */
				dead = TRUE;
				no_of_guns = 0;            /*game over */
				return;
			}

			n0 = rptr->neg_image[0];
			n1 = rptr->neg_image[1];
			p0 = rptr->tpos_image[image];
			p1 = rptr->bpos_image[image];

			for (c = 0; c < INV_COLS; c++) {
				iptr = &rptr->node[c];

				if (iptr->col[LEFT] != EMPTY) {
					/*erase existing invaders*/

					scrout(0, otl, iptr->col[LEFT], n0, ATTR_N);
					scrout(0, obl, iptr->col[LEFT], n1, ATTR_N);
					strcpy(&screen[otl][iptr->col[LEFT]], n0);
					strcpy(&screen[obl][iptr->col[LEFT]], n1);

					/*put up a new set*/

					scrout(0, tl, iptr->col[LEFT], p0, ATTR_N);
					scrout(0, bl, iptr->col[LEFT], p1, ATTR_N);
					strcpy(&screen[tl][iptr->col[LEFT]], p0);
					strcpy(&screen[bl][iptr->col[LEFT]], p1);
				}
			}
		}
	}
}

/**/

/*draws a screen full of invaders and sets up the invader data structure*/
/* initializes the following vars:                                      */
/*  cur_row                                                             */
/*  direction                                                           */
/*  no_of_invaders                                                      */
/*  invader missiles                                                    */
void init_screen()
{
	int l, c, i, j, k;
	struct invader *p;

	image = 0;
	direction = RIGHT;
	cur_row = INV_ROWS - 1;
	no_of_invaders = INV_ROWS*INV_COLS;
	gun_delay = GUN_DELAY;
	mmdelay = DELAY;
	for (i = 0; i < MAX_MISSILES; missile[i++].lin = EMPTY)
		;


	/*blank out the screen*/

	for (l = 0; l < MAX_LIN; l++)	{
		scrout(0, l, 0, blank, ATTR_N);
		for (c = 0; c < MAX_COL; c++)
			screen[l][c] = ' ';
	}

	/*a-type invaders*/

	alien[0].top_lin = 13;
	alien[0].bot_lin = 14;
	alien[0].tpos_image[0] = inva1a;
	alien[0].bpos_image[0] = inva2a;
	alien[0].tpos_image[1] = inva1b;
	alien[0].bpos_image[1] = inva2a;
	alien[0].neg_image[0] = nega;
	alien[0].neg_image[1] = nega;
	alien[0].value = VALUEA;
	alien[0].no_inv = INV_COLS;

	alien[1].top_lin = 10;
	alien[1].bot_lin = 11;
	alien[1].tpos_image[0] = inva1a;
	alien[1].bpos_image[0] = inva2a;
	alien[1].tpos_image[1] = inva1b;
	alien[1].bpos_image[1] = inva2a;
	alien[1].neg_image[0] = nega;
	alien[1].neg_image[1] = nega;
	alien[1].value = VALUEA;
	alien[1].no_inv = INV_COLS;

	for (j = 0; j < 2; j++) {
		for (i = 0; i < INV_COLS; i++) {
			p = &alien[j].node[i];
			p->col[LEFT] = i * 7 + (7 - inval) / 2 + (MAX_COL / INV_COLS) / 2;
			p->col[RIGHT] = p->col[LEFT] + inval - 1;
			for (k = 0; k < inval; k++)
			{
				screen[alien[j].top_lin][k + p->col[LEFT]] = inva1a[k];
				screen[alien[j].bot_lin][k + p->col[LEFT]] = inva2a[k];
			}
		}
	}

	/*b-type invaders*/

	alien[2].top_lin = 7;
	alien[2].bot_lin = 8;
	alien[2].tpos_image[0] = invb1a;
	alien[2].bpos_image[0] = invb2a;
	alien[2].tpos_image[1] = invb1b;
	alien[2].bpos_image[1] = invb2b;
	alien[2].neg_image[0] = negb;
	alien[2].neg_image[1] = negb;
	alien[2].value = VALUEB;
	alien[2].no_inv = INV_COLS;

	alien[3].top_lin = 4;
	alien[3].bot_lin = 5;
	alien[3].tpos_image[0] = invb1a;
	alien[3].bpos_image[0] = invb2a;
	alien[3].tpos_image[1] = invb1b;
	alien[3].bpos_image[1] = invb2b;
	alien[3].neg_image[0] = negb;
	alien[3].neg_image[1] = negb;
	alien[3].value = VALUEB;
	alien[3].no_inv = INV_COLS;

	for (j = 2; j < 4; j++) {
		for (i = 0; i < INV_COLS; i++) {
			p = &alien[j].node[i];
			p->col[LEFT] = i * 7 + (7 - invbl) / 2 + (MAX_COL / INV_COLS) / 2;
			p->col[RIGHT] = p->col[LEFT] + invbl - 1;
			for (k = 0; k < invbl; k++)
			{
				screen[alien[j].top_lin][k + p->col[LEFT]] = invb1a[k];
				screen[alien[j].bot_lin][k + p->col[LEFT]] = invb2a[k];
			}
		}
	}

	/*c-type invaders*/

	alien[4].top_lin = 1;
	alien[4].bot_lin = 2;
	alien[4].tpos_image[0] = invc1a;
	alien[4].bpos_image[0] = invc2a;
	alien[4].tpos_image[1] = invc1b;
	alien[4].bpos_image[1] = invc2b;
	alien[4].neg_image[0] = negc;
	alien[4].neg_image[1] = negc;
	alien[4].value = VALUEC;
	alien[4].no_inv = INV_COLS;

	j = 4;
	for (i = 0; i < INV_COLS; i++)	{
		p = &alien[j].node[i];
		p->col[LEFT] = i * 7 + (7 - invcl) / 2 + (MAX_COL / INV_COLS) / 2;
		p->col[RIGHT] = p->col[LEFT] + invcl - 1;
		for (k = 0; k < invcl; k++)
		{
			screen[alien[j].top_lin][k + p->col[LEFT]] = invc1a[k];
			screen[alien[j].bot_lin][k + p->col[LEFT]] = invc2a[k];
		}
	}

	/*draw the blocks*/

	for (k = 4; k < 65; k += 20)            /*4 blocks*/
		for (i = 0; i < 3; i++)              /*3 lines*/
			for (j = 0; j < 10; j++)            /*10 chars each*/
				screen[BLOCK_LIN + i][k + j] = block_image[i][j];

	for (i = 0; i < MAX_LIN - 1; i++) {
		screen[i][MAX_COL] = '\0';                      /*end of string*/
		scrout(0, i, 0, &screen[i][0], ATTR_N);           /*draw screen*/
	}
}

/**/

void scrout(int page, int x, int y, char *str, int attr)
{
	gotoxy(y + 1, x + 1);
	textattr(attr);
	_cputs(str);
}