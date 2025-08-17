#include <stdio.h>
#include <allegro.h>

#define WIDTH 640
#define HEIGHT 480

#define RIGHT 1
#define LEFT -1

const char int_to_bcd_7[] = {
    0b01110111,
    0b00100100,
    0b01011101,
    0b01101101,
    0b00101110,
    0b01101011,
    0b01111011,
    0b00100101,
    0b01111111,
    0b01101111
};

const int segment_position[][4] = {
    {0,0,30,5},
    {0,0,5,30},
    {30,0,5,30},
    {0,30,30,5},
    {0,30,5,30},
    {30,30,5,30},
    {0,60,30,5},
};

void draw_bcd_7(BITMAP *dest, int number, int x, int y, int color) {
    char value = int_to_bcd_7[number];
    for (int i = 0; i < 7; i++) {
        char current_segment = (value >> i) & 1;
        if (current_segment) {
            int x0 = x + segment_position[i][0];
            int y0 = y + segment_position[i][1];
            int x1 = x0 + segment_position[i][2];
            int y1 = y0 + segment_position[i][3];
            rectfill(dest, x0, y0, x1, y1, color);
        }
    }
}

typedef struct {
    int x;
    int y;
    int speed_x;
    int speed_y;
} moving_object;

typedef struct {
    int x;
    int y;
} paddle;

int bounce(moving_object *obj) {
    int score = 0;
    if (obj->x < 6) {
        int delta = 6 - obj->x;
        obj->x += 2 * delta;
        obj->speed_x *= -1;
        score = RIGHT;
    } else if (obj->x > WIDTH - 6) {
        int delta = obj->x - WIDTH + 6;
        obj->x -= 2 * delta;
        obj->speed_x *= -1;
        score = LEFT;
    }
    if (obj->y < 6) {
        int delta = 6 - obj->y;
        obj->y += 2 * delta;
        obj->speed_y *= -1;
    } else if (obj->y > HEIGHT - 6) {
        int delta = obj->y - HEIGHT + 6;
        obj->y -= 2 * delta;
        obj->speed_y *= -1;
    }
    return score;
}

int hit(moving_object *obj, paddle *obstacle) {
    return !(
        obj->x - 6 > obstacle->x + 8 ||
        obj->x + 6 < obstacle->x ||
        obj->y - 6 > obstacle->y + 60 ||
        obj->y + 6 < obstacle->y
        );
}

void game_loop() {
    int white = makecol(255,255,255);

    int points_left = 0;
    int points_right = 0;

    moving_object ball;
    ball.x = WIDTH/2;
    ball.y = HEIGHT/2;
    ball.speed_x = 10;
    ball.speed_y = -10;

    paddle left;
    left.x = 8;
    left.y = (HEIGHT/2)-60;

    paddle right;
    right.x = WIDTH-8-8;
    right.y = (HEIGHT/2)-60;

    BITMAP *buffer = create_bitmap(WIDTH, HEIGHT);
    if (!buffer) {
        allegro_exit();
        printf("Failed to create off screen buffer!");
        exit(-1);
    }

    while (!key[KEY_ESC]) {
        clear_to_color(buffer, makecol(0,0,0));
        //putpixel(screen, mouse_x, mouse_y, white);
        //draw_bcd_7(0, 10, 10, white);
        //draw_bcd_7(1, 50, 10, white);
        //draw_bcd_7(2, 90, 10, white);
        //draw_bcd_7(3, 130, 10, white);
        //draw_bcd_7(4, 170, 10, white);
        //draw_bcd_7(5, 210, 10, white);
        //draw_bcd_7(6, 250, 10, white);
        //draw_bcd_7(7, 290, 10, white);
        //draw_bcd_7(8, 330, 10, white);
        //draw_bcd_7(9, 370, 10, white);

        ball.x += ball.speed_x;
        ball.y += ball.speed_y;

        if (key[KEY_Q]) {
            left.y -= 11;
            if (left.y < 0) {
                left.y = 0;
            }
        } else if (key[KEY_A]) {
            left.y += 11;
            if (left.y+60 > HEIGHT) {
                left.y = HEIGHT-60;
            }
        }

        if (key[KEY_UP]) {
            right.y -= 11;
            if (right.y < 0) {
                right.y = 0;
            }
        } else if (key[KEY_DOWN]) {
            right.y += 11;
            if (right.y+60 > HEIGHT) {
                right.y = HEIGHT-60;
            }
        }

        if (hit(&ball, &left)) {
            ball.speed_x *= -1;
        } else if (hit(&ball, &right)) {
            ball.speed_x *= -1;
        }

        int score = bounce(&ball);
        if (score == RIGHT) {
            points_right++;
        } else if (score == LEFT) {
            points_left++;
        }

        if (points_right >= 10 || points_left >= 10) {
            break;
        }

        circlefill(buffer, ball.x, ball.y, 6, white);
        rectfill(buffer, left.x, left.y, left.x+8, left.y+60, white);
        rectfill(buffer, right.x, right.y, right.x+8, right.y+60, white);
        draw_bcd_7(buffer, points_left, 90, 10, white);
        draw_bcd_7(buffer, points_right, 540, 10, white);

        vsync();
        blit(buffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
    }

    destroy_bitmap(buffer);
}

int main() {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();
    enable_hardware_cursor();

    set_color_depth(16);
    int result = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, WIDTH, HEIGHT, 0, 0);
    if (result) {
        printf(allegro_error);
        allegro_exit();
        return result;
    }

    game_loop();

    allegro_exit();
}
END_OF_MAIN();