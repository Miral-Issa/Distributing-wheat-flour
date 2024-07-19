#include "local.h"

#define PI 3.14159265

#define MAX_PLANES 16  // Maximum number of planes
#define MAX_SQUARES 16 // Maximum number of squares per plane
float planeX[MAX_PLANES];
float planeY[MAX_PLANES];
float planeSpeed[MAX_PLANES];
int k = 0;

int numPlanes = 0;     // Default number of planes
int numContainers = 0; // Default number of squares per plane
int flag = 0;
int planeid = 0;
struct sharedOpenGL *memo;
int planeid;
int containerP_ID;
int containerC_ID;
float posY;
int received_flag = 0;
double good;
int numWeights = 0;
int reilef;
int ID1 = 0;
int energyFlag = 0;
int colID = 0;
int refillFlag = 0;
int family = 0;
int distributionFlag = 0;
int refill_pid = 0;
int fam_flag = 0;
int terminator = 0;
int refID;
void readShared()
{
    flag = memo->dropFLAG;
    containerP_ID = memo->cont->plane_pid;
    containerC_ID = memo->cont->containerID;
    received_flag = memo->recievedFlag;
    good = memo->cont->stillGood;
    numWeights = memo->wet;
    ID1 = memo->committeID;
    energyFlag = memo->energyCollect;
    refillFlag = memo->refillFLAG;
    family = memo->familyID;
    distributionFlag = memo->distributedFlag;
    refill_pid = memo->P_id;
    refillFlag = memo->refillFLAG;
    fam_flag = memo->familyflag;
    terminator = memo->terminatedFlag;
    refID = memo->refillID;
}
void drawContainers(int ID, int i)
{
    float squareSize = 0.1f / numContainers;
    float startY = planeY[i] - 0.07f;
    float space = 0.01f; // Adjust the space between squares
    for (int j = 0; j < numContainers; j++)
    {
        if (flag == 1 && containerP_ID == ID && containerC_ID == j && !refillFlag)
        {
            // Set startY to 0.1 if the condition is true
            glColor3f(1.0f, 0.0f, 0.0f); // Red color
        }
        else if (refillFlag)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
        }
        else
        {
            glColor3f(0.2f + i * 0.2f, 0.5f + i * 0.1f, 0.8f - i * 0.1f);
        }

        float startX = planeX[i] - 0.0f + j * (squareSize + space); // Calculate the starting x-coordinate for each square
        glBegin(GL_POLYGON);

        glVertex2f(startX, startY);
        glVertex2f(startX, startY - squareSize);
        glVertex2f(startX + squareSize, startY - squareSize);
        glVertex2f(startX + squareSize, startY);
        glEnd();
    }
}
void drawPlanes()
{
    // Draw all planes

    for (int i = 0; i < memo->numPlanes; i++)
    {
        if (memo->planeID[i] != planeid && !refillFlag)
        {
            // Set a specific color based on planeID
            glColor3f(0.2f + i * 0.2f, 0.5f + i * 0.1f, 0.8f - i * 0.1f);

            planeid = memo->planeID[i];
            drawContainers(memo->planeID[i], i);
        }
        else if (refillFlag == 1)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            // planeid = memo->planeID[i];
        }
        else
            glColor3f(0.2f + i * 0.2f, 0.5f + i * 0.1f, 0.8f - i * 0.1f);

        glBegin(GL_POLYGON);
        glVertex2f(planeX[i], planeY[i] + 0.05f);
        glVertex2f(planeX[i], planeY[i] - 0.05f);
        glVertex2f(planeX[i] + 0.1f, planeY[i]);
        glEnd();
    }
}
void SafeHouse()
{
    // Set the color to gray for the house
    glColor3f(0.5f, 0.5f, 0.5f);

    // Define the vertices of the house
    float width = 0.35f;
    float height = 0.2f;
    float roofHeight = 0.1f;
    float left = -0.9f + width;
    float right = -0.9f;
    float top = height;
    float bottom = -height;

    // Draw the house
    glBegin(GL_POLYGON);
    glVertex2f(left, bottom);
    glVertex2f(right, bottom);
    glVertex2f(right, top - roofHeight);
    glVertex2f(-0.9f + width / 2, top); // peak of the roof
    glVertex2f(left, top - roofHeight);
    glEnd();

    // Set the color to brown for the door
    glColor3f(0.6f, 0.3f, 0.0f);

    // Draw the door
    glBegin(GL_POLYGON);
    glVertex2f(-0.9f + width * 0.4f, bottom);
    glVertex2f(-0.9f + width * 0.6f, bottom);
    glVertex2f(-0.9f + width * 0.6f, bottom + 0.1f); // Adjust y-coordinate to be inside the house
    glVertex2f(-0.9f + width * 0.4f, bottom + 0.1f); // Adjust y-coordinate to be inside the house
    glEnd();

    // Draw the title "Safe House"
    glColor3f(0.0f, 0.0f, 0.0f);                            // Set color to black for the text
    glRasterPos2f(-0.98f + width / 2 - 0.05f, top - 0.13f); // Position the text above the house
    const char *title = "Gaza Safe House";
    for (int i = 0; title[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, title[i]);
    }
    glColor3f(1.0f, 1.0f, 1.0f);                       // Set color to white for the number
    glRasterPos2f(-0.9f + width / 2 - 0.015f, -0.04f); // Position the number in the middle of the house
                                                       // Display the tens digit
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (memo->wet / 10) + '0');

    // Display the ones digit
    glRasterPos2f(-0.9f + width / 2 + 0.015f, -0.04f); // Position the ones digit next to the tens digit
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (memo->wet % 10) + '0');
}
void PickUp_Containers()
{
    glPointSize(5.0f); // Set the point size to 5.0 (adjust as needed)

    glBegin(GL_POINTS);

    for (int i = 0; i < memo->committeeNum; i++)
    {
        if (received_flag == 1 && memo->committeIDs[i] == ID1)
        {
            // Color for flag 1 (e.g., red)
            float squareSize = 0.02f;
            float centerX = i * 0.2f;
            float centerY = 0.0f;
            glColor3f(1.0f, 0.0f, 0.0f); // Red color
            glVertex2f(centerX - squareSize / 2, centerY - squareSize / 2);
            glVertex2f(centerX + squareSize / 2, centerY - squareSize / 2);
            glVertex2f(centerX + squareSize / 2, centerY + squareSize / 2);
            glVertex2f(centerX - squareSize / 2, centerY + squareSize / 2);
            received_flag = 0;
        }
        else
        {
            // Color for other flags (e.g., blue)
            glColor3f(0.0f, 0.0f, 1.0f); // Blue color
        }

        // Draw the circle
        float radius = 0.05f;
        for (int j = 0; j < reilef; j++)
        {
            float theta = 2.0f * PI * j / reilef;
            float x = radius * cos(theta);
            float y = radius * sin(theta);
            glVertex2f(x + i * 0.2f, y);
        }
    }

    glEnd();
}
void drawFamily()
{
    float flagWidth = 0.1f;
    float flagHeight = 0.1f;
    float space = 0.08f; // Adjust the space between flags

    float startX = -0.4f; // Starting x-coordinate for the first flag

    for (int j = 0; j < memo->FamiliesNum; j++)
    {
        float startY = -0.7f; // Starting y-coordinate for each flag
        if (distributionFlag == 1 && j == family)
        {
            // Blue color for the flag when flag is 1
            glColor3f(1.0f, 0.5f, 0.0f); // Orange
        }
        else if (fam_flag)
        {
            // Blue color for the flag when flag is 1
            glColor3f(1.0f, 1.0f, 1.0f); // Orange
            fam_flag = 0;
        }
        else
        {
            // Colors of the Palestinian flag
            glColor3f(0.0f, 0.0f, 0.0f); // Black
            glBegin(GL_QUADS);
            glVertex2f(startX, startY);
            glVertex2f(startX + flagWidth, startY);
            glVertex2f(startX + flagWidth, startY + flagHeight / 3);
            glVertex2f(startX, startY + flagHeight / 3);
            glEnd();

            glColor3f(1.0f, 1.0f, 1.0f); // White
            glBegin(GL_QUADS);
            glVertex2f(startX, startY + flagHeight / 3);
            glVertex2f(startX + flagWidth, startY + flagHeight / 3);
            glVertex2f(startX + flagWidth, startY + 2 * flagHeight / 3);
            glVertex2f(startX, startY + 2 * flagHeight / 3);
            glEnd();

            glColor3f(0.0f, 0.5f, 0.0f); // Green
            glBegin(GL_QUADS);
            glVertex2f(startX, startY + 2 * flagHeight / 3);
            glVertex2f(startX + flagWidth, startY + 2 * flagHeight / 3);
            glVertex2f(startX + flagWidth, startY + flagHeight);
            glVertex2f(startX, startY + flagHeight);
            glEnd();

            glColor3f(1.0f, 0.0f, 0.0f); // Red
            glBegin(GL_TRIANGLES);
            glVertex2f(startX, startY + flagHeight / 3);
            glVertex2f(startX + flagWidth / 2, startY + flagHeight / 2);
            glVertex2f(startX, startY + 2 * flagHeight / 3);
            glEnd();
        }

        startX += flagWidth + space; // Update startX for the next flag
    }
}
void clearScreen()
{
    if (terminator == 1)
    {
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // Use "cls" for Windows and "clear" for Unix-like systems
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawPlanes();
    PickUp_Containers();
    SafeHouse();
    drawFamily();
    clearScreen();
    glutSwapBuffers();
}
void Update_time(int value)
{
    readShared();
    glutPostRedisplay();
    glutTimerFunc(2000 / 60, Update_time, 0); // Schedule the next update
}
void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    // Set initial positions and speeds for default number of planes
    for (int i = 0; i < numPlanes; i++)
    {
        planeX[i] = -0.2f + i * 0.2f; // Equally spaced along x-axis
        planeY[i] = 0.85f;
        planeSpeed[i] = 0.002f;
    }
}
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char **argv)
{
    key_t gl_shm_key = ftok(".", 'q');
    int shmid;

    if ((shmid = shmget(gl_shm_key, 0, 0)) != -1)
    {
        void *shmptr = shmat(shmid, NULL, 0);
        if (shmptr == (void *)-1)
        {
            perror("shmat -- OPENGL -- attach");
            exit(1);
        }
        memo = (struct sharedOpenGL *)shmptr;
    }
    else
    {
        perror("shmget -- OPEN -- access1");
        exit(2);
    }

    numPlanes = memo->numPlanes;
    numContainers = memo->numContainers;
    reilef = memo->reliefNum;
    memo->openGLID = getpid();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Project 2");
    glutReshapeFunc(reshape);
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, Update_time, 0);
    glutMainLoop();
    return EXIT_SUCCESS;
}
