#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 25
#define COLS 60
#define MAX_OBJECTS 50

struct Shape
{
    int type;       // 1-Line 2-Rectangle 3-Triangle 4-Circle
    int x1, y1;
    int x2, y2;
    int x3, y3;     // Triangle third vertex
    int radius;     // Circle radius
};

char canvas[ROWS][COLS];
struct Shape objects[MAX_OBJECTS];
int count = 0;

/* Initialize canvas with '_' */
void clearCanvas(void)
{
    int i, j;
    for(i = 0; i < ROWS; i++)
    {
        for(j = 0; j < COLS; j++)
        {
            canvas[i][j] = '_';
        }
    }
}

/* Plot a point '*' on the canvas */
void plot(int x, int y)
{
    if(x >= 0 && x < COLS && y >= 0 && y < ROWS)
    {
        canvas[y][x] = '*';
    }
}

/* Draw line using DDA algorithm */
void drawLine(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps;

    if(abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);

    if(steps == 0)
    {
        plot(x1, y1);
        return;
    }

    float xinc = dx / (float)steps;
    float yinc = dy / (float)steps;

    float x = x1;
    float y = y1;

    int i;
    for(i = 0; i <= steps; i++)
    {
        plot((int)round(x), (int)round(y));
        x += xinc;
        y += yinc;
    }
}

/* Draw rectangle */
void drawRectangle(int x1, int y1, int x2, int y2)
{
    drawLine(x1, y1, x2, y1);
    drawLine(x2, y1, x2, y2);
    drawLine(x2, y2, x1, y2);
    drawLine(x1, y2, x1, y1);
}

/* Draw triangle using three vertices */
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

/* Helper to plot points for 8-way symmetry in circle algorithm */
static void plotCirclePoints(int xc, int yc, int x, int y)
{
    plot(xc + x, yc + y);
    plot(xc - x, yc + y);
    plot(xc + x, yc - y);
    plot(xc - x, yc - y);
    plot(xc + y, yc + x);
    plot(xc - y, yc + x);
    plot(xc + y, yc - x);
    plot(xc - y, yc - x);
}

/* Draw circle using Bresenham's Midpoint Circle Algorithm */
void drawCircle(int xc, int yc, int r)
{
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    plotCirclePoints(xc, yc, x, y);
    while (y >= x)
    {
        x++;
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }
        plotCirclePoints(xc, yc, x, y);
    }
}

/* Redraw all stored objects on a clean canvas */
void redrawCanvas(void)
{
    int i;
    clearCanvas();

    for(i = 0; i < count; i++)
    {
        if(objects[i].type == 1)
        {
            drawLine(objects[i].x1, objects[i].y1,
                     objects[i].x2, objects[i].y2);
        }
        else if(objects[i].type == 2)
        {
            drawRectangle(objects[i].x1, objects[i].y1,
                          objects[i].x2, objects[i].y2);
        }
        else if(objects[i].type == 3)
        {
            drawTriangle(objects[i].x1, objects[i].y1,
                         objects[i].x2, objects[i].y2,
                         objects[i].x3, objects[i].y3);
        }
        else if(objects[i].type == 4)
        {
            drawCircle(objects[i].x1, objects[i].y1,
                       objects[i].radius);
        }
    }
}

/* Display Canvas to standard output */
void displayCanvas(void)
{
    int i, j;
    printf("\n");
    for(i = 0; i < ROWS; i++)
    {
        for(j = 0; j < COLS; j++)
        {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
}

/* Clear standard input buffer to prevent loops */
void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* List active shapes */
void listObjects(void)
{
    int i;
    if (count == 0)
    {
        printf("\nNo shapes created yet.\n");
        return;
    }
    printf("\n--- Active Shapes ---\n");
    for(i = 0; i < count; i++)
    {
        printf("[%d] ", i);
        if(objects[i].type == 1)
        {
            printf("Line: (%d, %d) to (%d, %d)\n", objects[i].x1, objects[i].y1, objects[i].x2, objects[i].y2);
        }
        else if(objects[i].type == 2)
        {
            printf("Rectangle: Corners (%d, %d) to (%d, %d)\n", objects[i].x1, objects[i].y1, objects[i].x2, objects[i].y2);
        }
        else if(objects[i].type == 3)
        {
            printf("Triangle: Vertices (%d, %d), (%d, %d), (%d, %d)\n", objects[i].x1, objects[i].y1, objects[i].x2, objects[i].y2, objects[i].x3, objects[i].y3);
        }
        else if(objects[i].type == 4)
        {
            printf("Circle: Center (%d, %d), Radius %d\n", objects[i].x1, objects[i].y1, objects[i].radius);
        }
    }
    printf("---------------------\n");
}

/* Warn user if coordinates are outside canvas */
int validateCoordinates(int x, int y)
{
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS)
    {
        printf("Warning: Coordinate (%d, %d) is outside the canvas boundary (0-%d, 0-%d). It will be clipped.\n", x, y, COLS - 1, ROWS - 1);
        return 0;
    }
    return 1;
}

/* Add a new shape object */
void addObject(void)
{
    int choice;
    if(count >= MAX_OBJECTS)
    {
        printf("Storage Full!\n");
        return;
    }

    printf("\n1. Line");
    printf("\n2. Rectangle");
    printf("\n3. Triangle");
    printf("\n4. Circle");
    printf("\nChoose Shape : ");

    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }

    if (choice < 1 || choice > 4)
    {
        printf("Invalid Choice.\n");
        return;
    }

    objects[count].type = choice;

    if(choice == 4)
    {
        printf("Enter center x y and radius: ");
        if (scanf("%d%d%d", &objects[count].x1, &objects[count].y1, &objects[count].radius) != 3)
        {
            printf("Invalid input format.\n");
            clearInputBuffer();
            return;
        }
        validateCoordinates(objects[count].x1, objects[count].y1);
        if (objects[count].radius < 0)
        {
            printf("Warning: Radius cannot be negative. Setting radius to 0.\n");
            objects[count].radius = 0;
        }
    }
    else if(choice == 3)
    {
        printf("Enter vertex 1 x y: ");
        if (scanf("%d%d", &objects[count].x1, &objects[count].y1) != 2) { printf("Invalid input.\n"); clearInputBuffer(); return; }
        printf("Enter vertex 2 x y: ");
        if (scanf("%d%d", &objects[count].x2, &objects[count].y2) != 2) { printf("Invalid input.\n"); clearInputBuffer(); return; }
        printf("Enter vertex 3 x y: ");
        if (scanf("%d%d", &objects[count].x3, &objects[count].y3) != 2) { printf("Invalid input.\n"); clearInputBuffer(); return; }
        validateCoordinates(objects[count].x1, objects[count].y1);
        validateCoordinates(objects[count].x2, objects[count].y2);
        validateCoordinates(objects[count].x3, objects[count].y3);
    }
    else
    {
        printf("Enter x1 y1 x2 y2: ");
        if (scanf("%d%d%d%d", &objects[count].x1, &objects[count].y1, &objects[count].x2, &objects[count].y2) != 4)
        {
            printf("Invalid input format.\n");
            clearInputBuffer();
            return;
        }
        validateCoordinates(objects[count].x1, objects[count].y1);
        validateCoordinates(objects[count].x2, objects[count].y2);
    }

    count++;
    redrawCanvas();
    printf("Shape added successfully!\n");
}

/* Delete an existing shape object */
void deleteObject(void)
{
    int index, i;

    if(count == 0)
    {
        printf("No objects present.\n");
        return;
    }

    listObjects();

    printf("Enter object number to delete (0 to %d): ", count - 1);
    if (scanf("%d", &index) != 1)
    {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }

    if(index < 0 || index >= count)
    {
        printf("Invalid index.\n");
        return;
    }

    for(i = index; i < count - 1; i++)
    {
        objects[i] = objects[i + 1];
    }

    count--;

    redrawCanvas();
    printf("Shape deleted successfully!\n");
}

/* Modify an existing shape object's coordinates */
void modifyObject(void)
{
    int index;

    if(count == 0)
    {
        printf("No objects present to modify.\n");
        return;
    }

    listObjects();

    printf("Enter object number to modify (0 to %d): ", count - 1);
    if (scanf("%d", &index) != 1)
    {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }

    if(index < 0 || index >= count)
    {
        printf("Invalid index.\n");
        return;
    }

    printf("\nRe-enter details for shape [%d]\n", index);

    if(objects[index].type == 4)
    {
        printf("Enter center x y and radius: ");
        if (scanf("%d%d%d", &objects[index].x1, &objects[index].y1, &objects[index].radius) != 3)
        {
            printf("Invalid input format.\n");
            clearInputBuffer();
            return;
        }
        validateCoordinates(objects[index].x1, objects[index].y1);
        if (objects[index].radius < 0)
        {
            printf("Warning: Radius cannot be negative. Setting radius to 0.\n");
            objects[index].radius = 0;
        }
    }
    else if(objects[index].type == 3)
    {
        printf("Enter vertex 1 x y: ");
        if (scanf("%d%d", &objects[index].x1, &objects[index].y1) != 2) { printf("Invalid input.\n"); clearInputBuffer(); return; }
        printf("Enter vertex 2 x y: ");
        if (scanf("%d%d", &objects[index].x2, &objects[index].y2) != 2) { printf("Invalid input.\n"); clearInputBuffer(); return; }
        printf("Enter vertex 3 x y: ");
        if (scanf("%d%d", &objects[index].x3, &objects[index].y3) != 2) { printf("Invalid input.\n"); clearInputBuffer(); return; }
        validateCoordinates(objects[index].x1, objects[index].y1);
        validateCoordinates(objects[index].x2, objects[index].y2);
        validateCoordinates(objects[index].x3, objects[index].y3);
    }
    else
    {
        printf("Enter x1 y1 x2 y2: ");
        if (scanf("%d%d%d%d", &objects[index].x1, &objects[index].y1, &objects[index].x2, &objects[index].y2) != 4)
        {
            printf("Invalid input format.\n");
            clearInputBuffer();
            return;
        }
        validateCoordinates(objects[index].x1, objects[index].y1);
        validateCoordinates(objects[index].x2, objects[index].y2);
    }

    redrawCanvas();
    printf("Shape modified successfully!\n");
}

int main(void)
{
    int choice;

    clearCanvas();

    do
    {
        printf("\n===== 2D GRAPHICS EDITOR =====");
        printf("\n1. Add Object");
        printf("\n2. Delete Object");
        printf("\n3. Modify Object");
        printf("\n4. Display Picture");
        printf("\n5. List Active Objects");
        printf("\n6. Exit");
        printf("\nEnter Choice : ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid selection. Please enter a number.\n");
            clearInputBuffer();
            choice = 0;
            continue;
        }

        switch(choice)
        {
            case 1:
                addObject();
                break;

            case 2:
                deleteObject();
                break;

            case 3:
                modifyObject();
                break;

            case 4:
                displayCanvas();
                break;

            case 5:
                listObjects();
                break;

            case 6:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid Choice\n");
        }

    } while(choice != 6);

    return 0;
}
