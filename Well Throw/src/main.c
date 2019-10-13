/////////////////////////////////////////////////////////
//                                                     //
//                      Well Throw                     //
//                                                     //
//                  Exam No - Y3848937                 //
//                                                     //
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
///////////////////////LIBRARTIES////////////////////////
/////////////////////////////////////////////////////////
#include <graphics_lib.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

/////////////////////////////////////////////////////////
/////////////////////////DEFINES/////////////////////////
/////////////////////////////////////////////////////////

/*The initial x and y positions of which graphics are drawn in relation to*/
#define INITIAL_X_POSITION 100
#define INITIAL_Y_POSITION 350

/*The x and y coordinates of the start of the projectile path*/
#define INITIAL_PROJECTILE_X 150
#define INITIAL_PROJECTILE_Y 295

/*Input angles need to be converted to radians using this constant*/
#define DEGTORAD 0.0174533

/* The acceleration due to gravity - See section 1 of documentation for more details*/
#define gravity 9.81

/////////////////////////////////////////////////////////
////////////////////GLOBAL VARIABLES/////////////////////
/////////////////////////////////////////////////////////
int total_Score = 0;/*The total score of each game, also used to determine
the difficulty of each level*/

/* velocity and angle are used to calculate the X and Y velocity of each throw*/
int velocity = 50;
int angle = 45;

/*The x coordinates of the randomly generated objects. bin_x and tree_x are global
variables initialised to be well outside of the graphics window's x axis to remove
a bug which caused random collisions being detected when they weren't on the screen*/
int Target_X;
int bin_x = 20000;
int tree_x = 20000;

/////////////////////////////////////////////////////////
///////////////////FUNCTION PTOTOTYPES///////////////////
/////////////////////////////////////////////////////////

/*Display Functions*/
void draw_target(void);
void draw_Well(x,y);
void draw_Man(x,y);
void draw_tree(x,y);
void draw_bin(x,y);
void draw_title(x,y);
void draw_Background(x,y);
void draw_world(x);
void draw_vel_controls(void);
void draw_ang_controls(void);
void draw_playagain_menu(void);
void display_throw_button(void);

/*Scoring Functions*/
int calculate_Score(int final_X);
void add_recent_score(void);
void get_recentscores(x,y);
void get_highscore(x,y);
void check_highscore(void);

/*Primary Functions*/
int draw_projectile(int initial_x, int initial_y, int vel_x, int vel_y);
void gameover(void);
void initialise_controls(void);
int throw_control(void);
int initialise_game(x,y);
void main_menu(void);

/////////////////////////////////////////////////////////
////////////////////Primary Functions////////////////////
/////////////////////////////////////////////////////////
int main(void)
{
    /*Graphics window opens immediately and loads the main menu.*/
    GFX_InitWindow(800, 480);
    main_menu();
    return 0;
}
int draw_projectile(int initial_x, int initial_y, int vel_x, int vel_y)
{

    float time;
    float x_position;
    float y_position;
    int hit; /*Used to check if there's been a collision with an obstacle*/

    /*Projectile coordinated are initialised to the startiong position of the throw*/
    y_position = initial_y;
    x_position = initial_x;

    GFX_SetColour(BLACK);
    GFX_MoveTo(INITIAL_PROJECTILE_X, INITIAL_PROJECTILE_Y);

    while(y_position < 410) /*This while loop detects for collisions between the projectile and the ground*/
    {
        /*As the x value is incremented, the y value is calculated and the arc is extended
        to the y value*/
        time = (x_position - initial_x)/ vel_x;//The time is calculated using the X coordinate...
        y_position = initial_y - (vel_y*time) + (gravity*time*time)/2;//...Which is used to calculate the Y coordinate
        GFX_DrawLineTo((int)x_position, (int)y_position, 3);
        ///////////////////////////////////////////////////
        //See section 2 of documentation for more details//
        ///////////////////////////////////////////////////
        x_position += 1.3;

        /*Collisions with the bin and tree obstacles are checked. The only way I managed to get
        this working was to usr 2 nested if statements instead of one single one*/
        if(((int)y_position > 350) && ((bin_x -25) < (int)x_position))
        {//If the projectile's coordinates are within a boundary around the bin, the while loop is broken and a hit is detected
            if (((int)x_position)< (bin_x + 25))
            {
                hit = 1;
                break;

            }
        }
        else
        {
            hit = 0;
        }
        if(((int)y_position > 250) && ((tree_x -35) < (int)x_position))
        {//And the same here for tree
            if (((int)x_position)< (tree_x + 25))
            {
                hit = 1;
                break;
            }
        }
        else
        {
            hit = 0;
        }
        GFX_UpdateDisplay();
    }

    //Now, the outcome of the throw is determined on whether ot not a hit was detected
    if (hit != 1)
    {
        draw_playagain_menu();
    }
    else
    {
        gameover();
    }


    return x_position;
}
void gameover(void)
{
    /*These lines are needed to fix a bug which caused the game to detect collisions with
    invisible obstacles when a game is restarted after a previous collision*/
    bin_x = 20000;
    tree_x = 20000;

    //Controls are hidden and the game over message is displayed
    GFX_DrawFilledRectangle(0, 65, 800, 120, LIGHTBLUE);
    GFX_DrawText(370, 68, "Game Over!");
    GFX_UpdateDisplay();

    //Then the score is saved as a recent score and checked against the high score
    check_highscore();
    add_recent_score();

    //After a 3 second pause, the game reverts back to the main menu
    sleep(3);
    main_menu();
}
int initialise_game(x,y)
{
    /*Firstly, the window needs to be cleared and prepared for a new throw*/
    GFX_ClearWindow();
    draw_world(x+310);
    draw_Man(x+9,y-22);
    get_highscore(20,20);
    display_score();



    srand(time(NULL)); //This line is needed in order to generate random numbers.

    /*For low scores below 1500, the target gets generated anywhere on the screen*/
    if( total_Score < 1500)
    {
        Target_X = GFX_RandNumber(250, 750);
        draw_target();

    }
    ///////////////////////////////////////////////////
    //See section 2 of documentation for more details//
    //      on how the obstacles are generated       //
    ///////////////////////////////////////////////////

    /*For scores between 1501 and 3500, the target is generated with a
    bin close to it*/
    else if ((total_Score >=1501) && (total_Score <= 3500))
    {
        Target_X = GFX_RandNumber(250, 750);
        if (Target_X < 500)
        {
            bin_x = GFX_RandNumber(Target_X + 50, Target_X + 200);
        }
        else if (Target_X > 500)
        {
            bin_x = GFX_RandNumber(250, Target_X - 200);
        }
        draw_target();

        draw_bin(bin_x, 387);
    }

    /*For higher scores above 3500, a tree is generated between the character and the
    target, as well as a bin on the far side of the target.*/
    else if (total_Score > 3500)
    {
        Target_X = GFX_RandNumber(400, 750);
        tree_x = GFX_RandNumber(300, Target_X - 100);
        bin_x = GFX_RandNumber(Target_X + 50, Target_X + 200);
        draw_tree(tree_x, 322);
        draw_target();
        draw_bin(bin_x, 387);
    }

    /*Controls are displayed and initialised*/
    display_throw_button();
    draw_vel_controls();
    draw_ang_controls();
    GFX_UpdateDisplay();
    initialise_controls();


}
int throw_control(void)
{
    int vel_x;
    int vel_y;
    int final_x; //The x coordinate that the projectile lands
    int score; //The score from the throw
    int distance;//Measured from the initial x coordinate of the projectile

    //The angles are converted to radians and the X and Y velocities are calculated.
    vel_x = velocity * cos(angle*DEGTORAD);
    vel_y = velocity * sin(angle*DEGTORAD);


    /*Next, the projectile is drawn and the total score is updated based on the throw and displayed*/
    final_x = draw_projectile(INITIAL_PROJECTILE_X, INITIAL_PROJECTILE_Y,vel_x, vel_y);
    score = calculate_Score(final_x);
    total_Score= total_Score + score;
    display_score();
    GFX_UpdateDisplay();

    return final_x;

}
void main_menu(void)
{
    int mouseX, mouseY;//Used to store the coordinates of the mouse
    int mouse_enable = 1; //Enables mouse clicks on the screen when 1

    //The graphical world is drawn
    draw_Background(350,250);
    draw_world(INITIAL_X_POSITION+310);

    //The title, instrusctions, high score and recent scores are displayed
    draw_title(395, 30);
    GFX_DrawRectangle(200, 120, 600, 170, 2);
    GFX_InitFont();
    GFX_DrawText(380, 138, "Play");
    GFX_DrawRectangle(200,190,600,240,2);
    GFX_DrawText(380,208, "Exit");
    GFX_DrawText(200, 65, "Welcome! Adjust the angle and velocity to throw ");
    GFX_DrawText(200, 77, "the coin as close to the well as possible. Try");
    GFX_DrawText(200, 89, "and avoid any obstacles. Good luck! ");
    GFX_DrawText(357, 250, "RECENT SCORES:");
    get_recentscores(350, 260);
    get_highscore(20, 20);
    GFX_UpdateDisplay();

    //An event queue is the created and the mouse initialised
    GFX_CreateEventQueue();
    GFX_InitMouse();
    GFX_RegisterMouseEvents();


    while(mouse_enable == 1)
    {
        GFX_WaitForEvent();
        if (GFX_IsEventMouseButton())
        { /*When the mouse is enabled, it waits for a mouse click and checks the
        coordinates*/
            GFX_GetMouseCoordinates(&mouseX, &mouseY);
            if ((200 < mouseX && mouseX < 600) && (120 < mouseY && mouseY < 170))
            { /*Clicking on the play button disables the mouse, resets the score and
            clears the window before starting a new game*/
                mouse_enable = 0;
                GFX_ClearWindow();
                total_Score = 0;
                initialise_game(INITIAL_X_POSITION, INITIAL_Y_POSITION);

            }
            else if ((200 < mouseX && mouseX < 600) && (190 < mouseY && mouseY < 240))
            { /*Clicking the exit button disables the mouse and closes the form*/
                mouse_enable = 0;
                GFX_CloseWindow();
            }
        }
    }
}
void initialise_controls(void)
{
    int mouseX, mouseY; //Used to store the coordinates of the first mouse click
    int mouseX2, mouseY2; //Used to store the coordinates of the second mouse click
    int control_check = 1;

    //Event queue is created and the mouse events are initialised for it
    GFX_CreateEventQueue();
    GFX_InitMouse();
    GFX_RegisterMouseEvents();


    while(control_check == 1) //control_check allows for multiple screen clicks
    {
        GFX_WaitForEvent();
        //Event queue waits for a click
        if (GFX_IsEventMouseButton())
        {
            GFX_GetMouseCoordinates(&mouseX, &mouseY);
            /*The coordinates of the click are saved and checked against the locations
            of buttons on the screen*/
            if ((30 < mouseX && mouseX < 40) && (80 < mouseY && mouseY < 100))
            {
            /*Clicking on the left pointing velocity arrow will decrease its value
            by 5*/
                if (velocity >=5)
                {
                    velocity =  velocity - 5;
                    GFX_DrawFilledRectangle(50,80,150,100,LIGHTBLUE);
                    draw_vel_controls();
                    GFX_UpdateDisplay();
                }
            }

            else if ((170 < mouseX && mouseX < 180) && (80 < mouseY && mouseY < 100))
            {
            /*Clicking on the right pointing velocity arrow will increase its value
            by 5*/
                if (velocity <= 170)
                {
                velocity =  velocity + 5;
                GFX_DrawFilledRectangle(50,80,179,100,LIGHTBLUE);
                draw_vel_controls();
                GFX_UpdateDisplay();
                }
            }
            else if ((350 < mouseX && mouseX < 420) && (70 < mouseY && mouseY < 110))
            {
            /*When the throw button is pressed, the projectile is drawn and all
            controls are disabled*/
                int i;

                i = throw_control();
                if (i != 0)
                {
                control_check = 0;
                }
            /*The game then waits for a second mouse click where the coordinates are
            based on mouseX2 and mouseY2. This controls the play again sub menu...*/
                int done = 1;
                while(done)
                {
                    GFX_WaitForEvent();
                    if (GFX_IsEventMouseButton())
                    {
                        GFX_GetMouseCoordinates(&mouseX2, &mouseY2);
                        if ((500 <mouseX2 && mouseX2 < 650) && (70 < mouseY2 && mouseY2< 85))
                        {
                        //...where clicking "play again" restarts the game...
                            GFX_ClearWindow();
                            initialise_game(INITIAL_X_POSITION, INITIAL_Y_POSITION);
                            GFX_UpdateDisplay();
                            done = 0;
                        }
                        else if ((500 <mouseX2 && mouseX2 < 650) && (100 < mouseY2 && mouseY2< 120 ))
                        {
                        /*...and clicking "main menu" saves the recent score and checks for a
                        new high score, before returning to the main menu*/
                            GFX_ClearWindow();
                            check_highscore();
                            add_recent_score();
                            main_menu();
                            GFX_UpdateDisplay();

                        }
                    }
                }
            }


            else if ((210 < mouseX && mouseX < 220) && (80 < mouseY && mouseY < 100))
            {
            /*Clicking on the left pointing angle arrow will decrease its value
            by 5*/

                if (angle > 5)
                {
                    angle =  angle - 5;
                    GFX_DrawFilledRectangle(221,80,319,100,LIGHTBLUE);
                    draw_ang_controls();
                    GFX_WaitForEvent();


                }
            }

            else if ((320 < mouseX && mouseX < 330) && (80 < mouseY && mouseY < 100))
            {
            /*Clicking on the right pointing velocity arrow will increase its value
            by 5*/
                if (angle <= 80)
                {
                    angle =  angle + 5;
                    GFX_DrawFilledRectangle(221,80,319,100,LIGHTBLUE);
                    draw_ang_controls();
                    GFX_UpdateDisplay();
                }
            }
        }


    }
}

/////////////////////////////////////////////////////////
////////////////////Display Functions////////////////////
/////////////////////////////////////////////////////////
void draw_target(void)
{
    int distance = (-(INITIAL_PROJECTILE_X) + Target_X); /*Distance variable used
    to calculate and display the distance from the man's arm to the target*/
    char Target_Text[20]; /*The character array used to display the distance text*/

    /*The well is displayed along with a vertical line used as a marker for the target*/
    draw_Well(Target_X - 15, 365);
    GFX_DrawLine(Target_X, 410, Target_X, 470, 2);

    /*Target_Text is assigned the string and distance variable and is displayed
    next to the target*/
    GFX_InitFont();
    GFX_SetColour(BLACK);
    sprintf(Target_Text, "Distance = %d", (int)distance);
    GFX_DrawText(Target_X - 125, 450, Target_Text);

}
void display_score(void)
{
    char score_text[20];/*A character array is set up to store the text to output*/

    /*Any previous score displayed is cleared by drawing a square over it and the
    character array is assigned a value and displayed on screen*/
    GFX_DrawFilledRectangle(50,50, 340, 80, LIGHTBLUE);
    GFX_InitFont();
    sprintf(score_text, "Total Score: = %d", total_Score);
    GFX_DrawText(20, 50, score_text);
}
void draw_Well(x,y)
{
    /*Draws the "well.png" bitmap at the x and y coordinates passed*/
    GFX_InitBitmap();
    BITMAP well = GFX_LoadBitmap("well.png"); /* Image size - 111x122px */
    GFX_DrawBitmap(well, x, y);
    GFX_UpdateDisplay();

}
void draw_Man(x,y)
{
    /*Draws the "man.png" bitmap at the x and y coordinates passed*/
    GFX_InitBitmap();
    BITMAP man = GFX_LoadBitmap("man.png"); /* Image size - 288x192px */
    GFX_DrawBitmap(man, x, y);
    GFX_UpdateDisplay();

}
void draw_tree(x,y)
{
    /*Draws the "tree.png" bitmap at the x and y coordinates passed*/
    GFX_InitBitmap();
    BITMAP tree = GFX_LoadBitmap("tree.png");/* Image size - 82x173px */
    GFX_DrawBitmap(tree, x, y);
    GFX_UpdateDisplay();

}
void draw_bin(x,y)
{
    /*Draws the "bin.png" bitmap at the x and y coordinates passed*/
    GFX_InitBitmap();
    BITMAP bin = GFX_LoadBitmap("bin.png"); /* Image size - 70*72px */
    GFX_DrawBitmap(bin, x, y);
    GFX_UpdateDisplay();

}
void draw_title(x,y)
{
    /*Draws the "title.png" bitmap at the x and y coordinates passed*/
    GFX_InitBitmap();
    BITMAP title = GFX_LoadBitmap("title.png");/* Image size - 263x54px */
    GFX_DrawBitmap(title, x, y);
    GFX_UpdateDisplay();

}
void draw_Background(x,y)
{
    /*A filled lightblue rectabgle covers the screen. This is used
    as a substitution for GFX_SetBackgroundColour as I couldn't get it to work*/
    GFX_DrawFilledRectangle(0,0,800,500,LIGHTBLUE);

    /*Then "Background.png" is drawn on top of it at the x and y
    coordinates passed*/
    GFX_InitBitmap();
    BITMAP background = GFX_LoadBitmap("Background.png");/* Image size - 960x556px */
    GFX_DrawBitmap(background, x, y);
    GFX_UpdateDisplay();

}
void draw_world(x)
{
    /*Background is drawn with a black rectangle and a green line at the bottom
    of the window representing the ground and a layer of grass respectively*/
    draw_Background(350,250);
    GFX_DrawFilledRectangle(0, x+6, 800, x+400, BROWN);
    GFX_SetColour(GREEN);
    GFX_DrawLine(0, x, 800, x, 5);
    /*This thick black line is also added between the grass and the ground to
    cover up occaisions when the path of the coin exceeds the top layer, making
    it look like the coin is going into the ground*/
    GFX_SetColour(BLACK);
    GFX_DrawLine(0, x + 7, 800, x + 7, 10);
}
void draw_vel_controls(void)
{
    //Triangles act as the buttons to control the velocity
    GFX_DrawFilledTriangle(40,100, 40, 80, 30, 90, BLUE);
    GFX_DrawFilledTriangle(170,100, 170, 80, 180, 90, BLUE);

    //Then a string is set up which displays the velocity in between the arrows
    GFX_InitFont();
    char vel_text[20];
    sprintf(vel_text, "Velocity: %d", velocity);
    GFX_DrawText(50, 80, vel_text);

    GFX_UpdateDisplay();
}
void draw_ang_controls(void)
{
    //Triangles act as the buttons to control the angle
    GFX_DrawFilledTriangle(220,100, 220, 80, 210, 90, BLUE);
    GFX_DrawFilledTriangle(320,100, 320, 80, 330, 90, BLUE);

    //Then a string is set up which displays the angle in between the arrows
    GFX_InitFont();
    char vel_text[20];
    sprintf(vel_text, "Angle: %d", angle);
    GFX_DrawText(230, 80, vel_text);

    GFX_UpdateDisplay();
}
void draw_playagain_menu(void)
{   //Displays 2 options after each throw
    GFX_DrawText(500, 70, "Next Level");
    GFX_DrawText(500, 100, "Main Menu");
    GFX_UpdateDisplay();
}

void display_throw_button(void)
{
    /*Disdplays the button to throw - a rectangle with text in the middle*/

    GFX_DrawRectangle(350, 70, 420, 110, 2);
    GFX_DrawText(360, 80, "Throw!");
    GFX_UpdateDisplay();

}

/////////////////////////////////////////////////////////
////////////////////Scoring Functions////////////////////
/////////////////////////////////////////////////////////
void get_recentscores(x,y)
{
    int  i=0,totalNums;
    double Scores[100];
    char line[100];  //Character array to store the contents of each line

    FILE *file;  //File pointer declared and recent scores file opened to read
    file = fopen("recentscores.txt", "r");

    /*This while loop moves through the document line by line, saving the contents to
    the line character array...*/
    while(fgets(line, sizeof line, file)!=NULL) {
        Scores[i]=atoi(line); /*...which is then converted from a string to an integer
        and saved in order in the scores array*/
        i++;
    }

    /*Each score value is formatted into a string and displayed as a numbered list*/
    char recentscore1[20];
    sprintf(recentscore1, "1.  %d", (int)Scores[0]);
    GFX_DrawText(x, y + 20 , recentscore1);

    char recentscore2[20];
    sprintf(recentscore2, "2.  %d", (int)Scores[1]);
    GFX_DrawText(x,y + 40 , recentscore2);
    fclose(file);

    char recentscore3[20];
    sprintf(recentscore3, "3.  %d", (int)Scores[2]);
    GFX_DrawText(x, y+ 60 , recentscore3);

    char recentscore4[20];
    sprintf(recentscore4, "4.  %d", (int)Scores[3]);
    GFX_DrawText(x, y+ 80 , recentscore4);

}
void get_highscore(x,y)
{
    FILE * file; //File pointer declared and highscores file opened to read
    file = fopen("highscore.txt", "r");
    int value;
    char line[100];

    while(!feof(file)) //The high score is saved in the line character array
    {
        fgets(line, 100, file);
        puts(line);
    }
    value = atoi(line); //Which is then converted to an integer and saved as value...
    char highscorevalue[20];
    sprintf(highscorevalue, "HIGH SCORE =  %d", (int)value); /*...This then gets formatted
    as a string and displayed*/
    GFX_DrawText(x, y , highscorevalue);
    fclose(file);
}
void check_highscore(void)
{
    FILE * file; //File pointer declared and highscores file opened to read
    file = fopen("highscore.txt", "r");
    double value;
    char line[150];

    while(!feof(file))//The high score is saved in the line character array
    {
        fgets(line, 150, file);
        puts(line);
    }
    value = atoi(line);//Which is then converted to an integer and saved as value...
    fclose(file);

    if ((int)value < total_Score)/*The saved high score is then checked to see if it's greater
    than the game's highest score...*/
    {
    FILE * file;/*...which, if true, reopens the file to write the new score*/
    file = fopen("highscore.txt", "w");
    truncate(file, 0);
    fprintf(file, "%d", total_Score);
    fclose(file);
    }
}
int calculate_Score(int final_X)
{

    int score;
    int temp;

    /*The positive difference between the Target's x position and the final
    x position of the coin is calculated and saved in temp*/
    temp = abs(abs(Target_X)- abs(final_X));

    /*The value of temp is then subtracted from 1000, so the smaller the
    closer the throw to the target, the closer the score is to 1000*/
    score = 1000-temp;
    return score;

}
void add_recent_score(void)
{
///////////////////////////////////////////////////
//See section 3 of documentation for more details//
///////////////////////////////////////////////////
    int  i=0;
    int Scores[10];
    char line[100];

    FILE *file; //Recent scores file is opened to read
    file = fopen("recentscores.txt", "r");

    while(fgets(line, sizeof line, file)!=NULL)
    {/*The contents of each line is saved to the line character array which is then
    saved as an integerin the scores array*/
        Scores[i]=atoi(line);
        i++;
    }
    fclose(file);

    /*Each score is then moved down one position in the array and the total score is
    added at the top*/
    Scores[3] = Scores[2];
    Scores[2] = Scores[1];
    Scores[1] = Scores[0];
    Scores[0] = (int)total_Score;

    //Recent scores file is reopened to write
    file = fopen("recentscores.txt", "w");
    /*The contents of the new score array is now rewritten over the previous text in
    the file*/
    fprintf(file, "%d\n", Scores[0]);
    fprintf(file, "%d\n", Scores[1]);
    fprintf(file, "%d\n", Scores[2]);
    fprintf(file, "%d\n", Scores[3]);
    fclose(file);
}
