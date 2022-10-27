#include <graphics_lib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

float gravity;
double x_map , y_map;
float i_angle = 10;
float i_velocity = 10;
float pos_x = 210;
float pos_y = 900;
float * i_velocity_ptr = &i_velocity; // pointer
float * i_angle_ptr = &i_angle; // pointer
int x_Moon , y_Moon; // random coordinates of the moon
int Mouse_x , Mouse_y;
int check;
int button;
int key;

void Draw_Motion(float i_angle_ptr, float i_velocity_ptr, float x_Moon, float y_Moon); /* Drawing the movement of rocket*/
void Draw_Rocket(int pos_x, int pos_y); /* Drawing the rocket */
void Draw_Moon(int x_Moon, int y_Moon); /* Drawing the Moon */
void Draw_Map(double x_map, double y_map); /* Drawing the map (including the earth, the mars, the controller and the background) for the game */
void Velocity_angle_controller(float i_velocity, float i_angle); /* Show the value for the velocity and the angle controller */

double Find_angle(double pos_x, double pos_y);
double Find_d_rocket_moon(double x_Moon, double y_Moon, double pos_x, double pos_y); /* d = distance */
double Find_a_rocket_moon(double x_Moon, double y_Moon, double pos_x, double pos_y); /* a = angle */
double Find_d_mars_rocket(double pos_x, double pos_y); /* d = distance */
double i_horizontal_velocity(double i_angle_ptr, double i_velocity_ptr);
double i_vertical_velocity(double i_angle_ptr, double i_velocity_ptr);

BITMAP Background, Scenarioo, Mainmenu, Rocket, Moon, Earth, Mars, Buzzandwoody;

void Game_play(); /* able to adjust the value of velocity and angle controller by clicking the cursor and pressing keyboards to launch, quit or re-play */
void Main_menu(); /* Main menu of the game*/
void Draw_playscreen(); /* draw the background, the earth, the mars, the rocket and the moon */
void Draw_mainmenu(); /* Bring the main menu picture */


int main(void) /* The main function */
{
    GFX_InitWindow(1500, 1000); // Open a graphics window and make it 1500 pixels wide by 1000 pixels high

    srand(time(NULL)); // Placing the moon randomly on the screen
    x_Moon = GFX_RandNumber(600,1010); // within  this inclusive x-ranges
    y_Moon = GFX_RandNumber(350,675);  // within  this inclusive y-ranges

    Buzzandwoody = GFX_LoadBitmap("Buzzandwoody.bmp");
    Background = GFX_LoadBitmap("Background.bmp");
    Scenarioo = GFX_LoadBitmap("Scenarioo.bmp");
    Mainmenu = GFX_LoadBitmap("Mainmenu.bmp");
    Rocket = GFX_LoadBitmap("Rocket.bmp");
    Earth = GFX_LoadBitmap("Earth.bmp");
    Moon = GFX_LoadBitmap("Moon.bmp");
    Mars = GFX_LoadBitmap("Mars.bmp");

    Main_menu();
    Draw_playscreen();
    Game_play();

    getchar();
    getchar();

    GFX_CloseWindow(); // remove the display

    return 0;
}

void Draw_Map(double x_map, double y_map) /* Drawing the map (including the earth, the mars, the controller and the background) for the game */
{
    GFX_ClearWindow();

    GFX_DrawBitmap(Background, 750, 500); // Drawing the background at their centre point 750, 500
    GFX_DrawBitmap(Earth, 90, 910);       // Drawing the the earth at their centre point 90, 910
    GFX_DrawBitmap(Mars, 1350, 120);      // Drawing the the mars at their centre point 1350, 120

    GFX_InitFont();
    GFX_DrawText(37, 690, "VELOCITY"); // Drawing text to indicate that it is velocity controller
    GFX_DrawText(170, 690, "ANGLE");   // Drawing text to indicate that it is angle controller
    GFX_DrawText(37, 350, "Press 'L' to launch the rocket");
    GFX_DrawText(37, 400, "Press 'ENTER' to re-start the game");
    GFX_DrawText(37, 450, "Press 'SPACE  BAR' to quit");
    GFX_DrawFilledTriangle(35, 725,  35, 715,  25, 720, YELLOW);   // left triangle for velocity controller
    GFX_DrawFilledTriangle(100, 725,  100, 715, 110, 720, YELLOW); // right triangle for velocity controller
    GFX_DrawFilledTriangle(160, 725,  160, 715, 150, 720, YELLOW); // left triangle for angle controller
    GFX_DrawFilledTriangle(220, 725,  220, 715, 230, 720, YELLOW); // right triangle for angle controller
}

void Draw_Moon(int x_Moon, int y_Moon) /* Drawing the Moon */
{
    GFX_DrawBitmap(Moon, x_Moon, y_Moon);
}

void Draw_Rocket(int pos_x, int pos_y) /* Drawing the rocket */
{
    GFX_DrawBitmap(Rocket, pos_x, pos_y);
}

void Draw_Motion(float i_angle_ptr, float i_velocity_ptr, float x_Moon, float y_Moon) /* the movement of rocket*/
{
    i_angle_ptr = i_angle_ptr*(2*M_PI/360); // Changing the value of angle in degree to radian

    double horizontal_velocity = i_horizontal_velocity(i_angle_ptr, i_velocity_ptr);
    double vertical_velocity = i_vertical_velocity(i_angle_ptr, i_velocity_ptr);

    double dbrm;
    double dmr;

    GFX_MoveTo(pos_x, pos_y);

    while (pos_y <= 900 && pos_y >= 0 && pos_x <= 1500 && pos_x >= 0) /* when the rocket is within this range */
    {
        pos_x = horizontal_velocity + pos_x; // the final horizontal position of the rocket = current horizontal velocity of the rocket + initial horizontal position of the rocket
        pos_y = -vertical_velocity + pos_y; // the final vertical position of the rocket = current vertical velocity of the rocket + initial vertical position of the rocket

        Draw_Rocket(pos_x, pos_y); // The rocket moves using the values of pos_x and pos_y above.

        dbrm = Find_d_rocket_moon(x_Moon, y_Moon, pos_x, pos_y);
        dmr = Find_d_mars_rocket(pos_x, pos_y);

        if (dbrm <= 250) /* There's gravitational force acts on the rocket if distance between rocket and the moon is smaller/equal to 250.
                            The radius of influence zone of the moon should be 250 which is 5 times the radius of the moon (=50) */
        {
            float gravity = (1.62*50/dbrm); // 1.62 is the gravitational acceleration. Then multiply it with the radius of the moon
                                            // and divide it by the distance between the rocket and the centre of the moon so that
                                            // the gravity is inversely proportional to the distance. when the rocket is nearer to the moon, the gravitational acceleration gets stronger.
            double a_rc;

            a_rc = Find_a_rocket_moon(x_Moon, y_Moon, pos_x, pos_y);

            horizontal_velocity = horizontal_velocity + (gravity*cos(a_rc));
            vertical_velocity = vertical_velocity - (gravity*sin(a_rc));

            if (dbrm <= 50) /* If the distance betwen the rocket and the moon is smaller than /equal to 50, it stops the loop */
            {
                GFX_ClearWindow();
                Draw_playscreen();
                GFX_DrawText(x_Moon+80, y_Moon, "I'm surrounded by"); // This font comes out when the rocket lands on the moon
                GFX_DrawText(x_Moon+80, y_Moon+15, "alien creatures!");
                GFX_UpdateDisplay(); // move the contents of the screen buffer to the display
                break;
            }
        }
        else if (dmr <= 90) /* If the distance between the mars and the rocket is smaller than /equal to 90, it stops the loop*/
            {
                GFX_ClearWindow();
                Draw_playscreen();
                GFX_DrawText(1140, 120, "You've got a"); // This font comes out when the rocket lands on the mars
                GFX_DrawText(1140, 140, "friend in me!");
                GFX_UpdateDisplay();
                break;
            }
        if (pos_y >= 900 || pos_y <= 0 || pos_x >= 1500 || pos_x <= 0)
        {
            GFX_ClearWindow();
            Draw_playscreen();
            GFX_DrawText(350, 720, "Don't give up, mate!");
            GFX_DrawBitmap(Buzzandwoody, 460, 810);
            GFX_UpdateDisplay();
            break;
        }
        else
        {
            gravity = 0; /* For other range, the gravitational force acts on the rocket is zero */
        }
        GFX_ClearWindow();
        Draw_playscreen();
        GFX_UpdateDisplay();
    }

}

double i_horizontal_velocity(double i_angle_ptr, double i_velocity_ptr) /* Find the horizontal velocity = vx */
{
    double vx;
    vx = i_velocity_ptr*cos(i_angle_ptr); // Calculate the horizontal velocity
    return vx;
}

double i_vertical_velocity(double i_angle_ptr, double i_velocity_ptr) /* Find the vertical velocity = vy */
{
    double vy;
    vy = i_velocity_ptr*sin(i_angle_ptr);  // Calculate the vertical velocity
    return vy;
}

double Find_d_rocket_moon(double x_Moon, double y_Moon, double pos_x, double pos_y) /* Find the distance between the rocket and the centre of the moon */
{
    double dbrm;
    dbrm = sqrt(pow((x_Moon - pos_x),2) + pow((y_Moon - pos_y),2));

    return dbrm;

}

double Find_d_mars_rocket(double pos_x, double pos_y)  /* Find the distance between the centre of the mars and the rocket */
{
    double dmr;
    dmr = sqrt(pow((1350 - pos_x),2) + pow((120 - pos_y),2)); // The centre point of the mars is (1350,120). The coordinate of the rocket is (pos_x , pos_y)
                                                              //  Calculate the distance using pythagoras theorem.
    return dmr;

}

double Find_a_rocket_moon(double x_Moon, double y_Moon, double pos_x, double pos_y) /* Find the angle between the rocket and the centre of the moon  */
{
    double x_c = x_Moon-pos_x; // Calculate the vertical distance
    double y_c = y_Moon-pos_y; // Calculate the horizontal distance
    double a_rc;
    a_rc = atan2(y_c, x_c); // Calculate the angle using the equation tan(angle) = opposite / adjacent.
    return a_rc;
}

void Draw_velocity_angle_controller(float i_velocity, float i_angle) /* Show the value for the velocity and the angle controller */
{
    char v_text[10];
    sprintf(v_text, "%.1fl", i_velocity); // converting i_velocity into a string, v_text
    GFX_DrawText(50, 712, v_text); // Drawing the value of i_velocity at 50, 273

    char a_text[10];
    sprintf(a_text,"%.1fl", i_angle); // converting i_angle into a string, a_text
    GFX_DrawText(172, 712, a_text); // Drawing the value of i_angle at 172, 723
}

void Draw_playscreen() /* Draw the map (Background, the earth, the mars), the rocket and the moon */
{
    Draw_Map(x_map,y_map); // Background, the earth, the mars
    Draw_Moon(x_Moon, y_Moon); // Drawing the moon
    Draw_Rocket(pos_x, pos_y); // Drawing the rocket
}

void Draw_mainmenu() /* Drawing menu */
{
    Mainmenu = GFX_LoadBitmap("Mainmenu.bmp"); // Load the main menu screen
    GFX_DrawBitmap(Mainmenu, 750, 500);
}

void Game_play() /* While playing the game, adjusting velocity and angle by clicking controller and pressing keyboards to launch, quit and re-play */
{
    int check = 1;

    Draw_playscreen(); // Draw the map (Background, the earth, the mars), the rocket and the moon for the background of the game
    GFX_UpdateDisplay();

    GFX_CreateEventQueue(); // Set up event queue, keyboard and mouse
    GFX_InitMouse();
    GFX_RegisterMouseEvents();
    GFX_InitKeyboard();
    GFX_RegisterKeyboardEvents();

    while(check == 1)
    {
        GFX_WaitForEvent(); // Wait for an event

        if (GFX_IsEventMouseButton())
        {
            GFX_GetMouseButton(&button); /* Check it’s left click */

                if (button == MOUSE_BUTTON_LEFT)
                {
                    GFX_GetMouseCoordinates(&Mouse_x, &Mouse_y); // Get click coordinates and work according to the loops

                    if (Mouse_x > 25 && Mouse_x < 35 && Mouse_y > 715 && Mouse_y < 725) /* If the user clicks within this range, they are changing the value of velocity*/
                    {
                        if (i_velocity >=10) /* the value of i_velocity is greater than or equal to 10 */
                        {
                            i_velocity = i_velocity - 5; // the velocity is deducted by 5 when clicking the left triangle for velocity controller
                            Draw_playscreen();   // Draw the map( Background, the earth, the mars), the rocket and the moon
                            Draw_velocity_angle_controller(i_velocity, i_angle); // Show the value for the velocity and the angle controller

                            GFX_UpdateDisplay();
                        }
                    }
                    else if (Mouse_x > 100 && Mouse_x < 110 && Mouse_y > 715 && Mouse_y < 725) /* If the user clicks within this range, they are changing the value of velocity*/
                    {
                        if (i_velocity <=45) /* the value of i_velocity is smaller than/equal to 45 */
                        {
                            i_velocity = i_velocity + 5; // the velocity is added by 5 when clicking the right triangle for velocity controller
                            Draw_playscreen();
                            Draw_velocity_angle_controller(i_velocity, i_angle);
                            GFX_UpdateDisplay();
                        }
                    }
                    else if (Mouse_x > 150 && Mouse_x < 160 && Mouse_y > 715 && Mouse_y < 725) /* If the user clicks within this range, they are changing the value of angle */
                    {
                        if (i_angle >= 10) /* the value of i_velocity is greater then/equal to 10 */
                        {
                            i_angle =  i_angle - 5; // the angle is deducted by 5 when clicking the left triangle for angle controller
                            Draw_playscreen();
                            Draw_velocity_angle_controller(i_velocity, i_angle);
                            GFX_UpdateDisplay();
                        }
                    }

                    else if (Mouse_x > 220 && Mouse_x < 230 && Mouse_y > 715 && Mouse_y < 725) /* If the user clicks within this range, they are changing the value of angle */
                    {
                        if (i_angle <= 75) /* the value of i_velocity is smaller then/equal to 10 */
                        {
                            i_angle =  i_angle + 5; // the angle is added by 5 when clicking the right triangle for angle controller
                            Draw_playscreen();
                            Draw_velocity_angle_controller(i_velocity, i_angle);
                            GFX_UpdateDisplay();
                        }
                    }


                }
        }

        else if (GFX_IsEventKeyDown())
        {
            GFX_GetKeyPress(&key);
            switch(key)
            {
                case(84): /* press upkey to launch the rocket */

                Draw_playscreen();  // Draw the map (Background, the earth, the mars), the rocket and the moon
                Draw_velocity_angle_controller(i_velocity, i_angle); // Show the value for the velocity and the angle controller
                Draw_Motion(*i_angle_ptr, *i_velocity_ptr, x_Moon, y_Moon); // Drawing the movement of rocket
                break;

                case(67): /* press enterkey to re-start the game */

                pos_x = 210; // pos_x and pos_y should be constant so that the rocket will comeback to it's original position when enter is pressed.
                pos_y = 900; // If not, the rocket will continue to move at the position where it ended.
                srand(time(NULL)); // When re-start the game, the moon will appear in the different position
                x_Moon = GFX_RandNumber(600,1010);
                y_Moon = GFX_RandNumber(350,675);
                Draw_playscreen();
                Draw_velocity_angle_controller(i_velocity, i_angle);
                GFX_UpdateDisplay();
                break;

                case(75): /* press spacebar to quit */
                GFX_CloseWindow();
                break;

            }
        }
    }
        *i_velocity_ptr = i_velocity; // Using pointer to target i_velocity to i_velocity_ptr
        *i_angle_ptr = i_angle; // Using pointer to target i_angle to i_angle_ptr
}

void Main_menu() /* Main menu for the game */
{
    int check = 0;

    GFX_CreateEventQueue(); // Set up event queue, keyboard and mouse
    GFX_InitMouse();
    GFX_RegisterMouseEvents();
    Draw_mainmenu(); // Drawing menu
    GFX_UpdateDisplay();

    while (check == 0)
        {
            int start = 0;
            GFX_WaitForEvent(); // Wait for an event

            if (GFX_IsEventMouseButton())
            {
                GFX_GetMouseButton(&button); // Check it’s left click

                if (button == MOUSE_BUTTON_LEFT)
                {
                    GFX_GetMouseCoordinates(&Mouse_x, &Mouse_y); // Get click coordinates and work according to the loops

                    if (Mouse_x > 100 && Mouse_x < 480 && Mouse_y > 700 && Mouse_y < 750) /* GAME START */
                    {
                        check = 1; // when the check is 1, it escape this while loop and the game start
                        GFX_ClearWindow();
                        Draw_playscreen(); // Draws the map (Background, the earth, the mars), the rocket and the moon
                        Game_play(); // This function is able to adjust the value of velocity and angle controller by clicking and pressing keyboards to launch, quit or re-play
                        GFX_UpdateDisplay();
                    }

                    else if (Mouse_x > 100 && Mouse_x < 420 && Mouse_y > 790 && Mouse_y < 835) /* SCENARIO */
                    {
                        start = 1; // when the start is 1, it quits this loop and goes to another while loop where start is 1.
                        Scenarioo = GFX_LoadBitmap("Scenarioo.bmp");
                        GFX_DrawBitmap(Scenarioo, 750, 500);
                        GFX_UpdateDisplay();
                    }
                    else if (Mouse_x > 100 && Mouse_x < 255 && Mouse_y > 880 && Mouse_y < 970) /* QUIT */
                    {

                        GFX_CloseWindow(); //remove the display and exit the game
                    }
                }
                while (start ==1)
                {
                    GFX_DrawBitmap(Scenarioo, 750, 500); // Draws scenario
                    GFX_UpdateDisplay();
                    GFX_WaitForEvent();

                    if (GFX_IsEventMouseButton())
                    {
                        GFX_GetMouseButton(&button);

                        if (button == MOUSE_BUTTON_LEFT) /* Check it’s left click */
                        {
                            GFX_GetMouseCoordinates(&Mouse_x, &Mouse_y);

                            if (Mouse_x > 80 && Mouse_x < 250 && Mouse_y > 740 && Mouse_y < 970) /* when the user clicks this range, the page goes back to the main menu */
                            {
                            start = 0;
                            Draw_mainmenu(); // Draw the main menu
                            Main_menu();
                            GFX_UpdateDisplay();
                            }

                        }
                    }
                }
            }
        }
}
