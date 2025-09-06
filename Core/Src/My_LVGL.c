#include "My_LVGL.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <time.h> // For time functions if needed

void my_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map)
{
    // 1. 计算要传输的总像素数
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    int32_t total_pixels_to_flush = w * h;

    // 2. 设置LCD硬件的刷新窗口
    LCD_SetWindows(area->x1, area->y1, area->x2, area->y2);

    // 3. 准备开始传输
    LCD_CS_CLR;
    LCD_RS_SET;

    // 将LVGL的像素缓冲区指针 (px_map) 转换为16位颜色指针
    uint16_t *color_p = (uint16_t *)px_map;

    // 4. 分块进行传输
    while (total_pixels_to_flush > 0)
    {
        // 计算当前块能传输多少像素
        int32_t chunk_pixel_count = total_pixels_to_flush;
        if (chunk_pixel_count > OneStepSize)
        {
            chunk_pixel_count = OneStepSize;
        }
        // **核心：进行颜色格式转换 (RGB565 -> RGB666)**
        uint16_t pixel;
        uint8_t *dma_ptr = dma_buffer;

        for (int32_t i = 0; i < chunk_pixel_count; i++)
        {
            *dma_ptr++ = (*color_p >> 8) & 0xF8; // R
            *dma_ptr++ = (*color_p >> 3) & 0xFC; // G
            *dma_ptr++ = (*color_p << 3);        // B
            color_p++;
        }
        // 等待上一次DMA传输完成

        while (!spi_dma_tx_complete)
            ;

        // 清除标志位，准备开始新的DMA传输
        spi_dma_tx_complete = 0;

        // ***** 关键步骤：在启动DMA前，清理D-Cache *****
        // 确保 dma_buffer 中的新数据被写入到主内存
        SCB_CleanDCache_by_Addr((uint32_t *)dma_buffer, chunk_pixel_count * 3);

        // 启动DMA传输
        HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, chunk_pixel_count * 3);

        // 更新剩余像素计数和颜色指针
        total_pixels_to_flush -= chunk_pixel_count;
        color_p += chunk_pixel_count;
    }

    // 5. 等待最后一块数据传输完成
    while (!spi_dma_tx_complete)
        ;

    // 6. 传输完成，拉高片选
    LCD_CS_SET;

    // 7. **非常重要**：通知LVGL刷新完成
    lv_display_flush_ready(display);
}

void my_input_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (TP_Scan(0))
    {
        data->point.y = (tp_dev.x) * 0.08;
        data->point.x = tp_dev.y * 0.12;
        // My_Usart_Send_Num((int)data->point.x);
        // My_Usart_Send_Num((int)data->point.y);
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}


// Assuming these are declared globally or passed appropriately
 
static void main_menu_event_handler(lv_event_t * e);
static void back_to_main_event_handler(lv_event_t * e);
static lv_obj_t * main_screen;
static lv_obj_t * screen_1;
static lv_obj_t * screen_2;
static lv_obj_t * screen_3;
static lv_obj_t * screen_4;

// Pointers to the labels for dynamic updates
static lv_obj_t * time_label = NULL;
static lv_obj_t * date_label = NULL;
static lv_obj_t * weather_label = NULL;
static lv_obj_t * welcome_label = NULL;

// --- Animation Handler ---
static void anim_y_handler(void * var, int32_t v) {
    lv_obj_set_style_translate_y((lv_obj_t *)var, v, 0);
}

// --- Function to Start Welcome Animation ---
static void start_welcome_animation(lv_obj_t * obj) {
    if (!obj) return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, -5, 5); // Move 5 pixels up and down
    lv_anim_set_duration(&a, 1500); // 1.5 seconds for one direction
    lv_anim_set_playback_delay(&a, 0);
    lv_anim_set_playback_duration(&a, 1500); // 1.5 seconds back
    lv_anim_set_repeat_delay(&a, 500); // 0.5 second pause at end points
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // Loop forever
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out); // Smooth easing
    lv_anim_set_exec_cb(&a, anim_y_handler); // Use custom handler
    lv_anim_start(&a);
}

// --- Function to Update Dynamic Information ---
// Call this function whenever time, date, weather, or welcome message changes
void update_main_screen_info(const char * new_time, const char * new_date, const char * new_weather, const char * new_welcome_msg) {
    if (time_label) {
        lv_label_set_text(time_label, new_time ? new_time : "00:00:00");
    }
    if (date_label) {
        lv_label_set_text(date_label, new_date ? new_date : "1970-01-01");
    }
    if (weather_label) {
        lv_label_set_text(weather_label, new_weather ? new_weather : "Sunny");
    }
    if (welcome_label) {
        lv_label_set_text(welcome_label, new_welcome_msg ? new_welcome_msg : "Welcome!");
        // Ensure animation is running if label text changes (optional, if animation stops)
        // start_welcome_animation(welcome_label);
    }
}

// --- Main Screen Creation Function ---

void create_main_screen(void) {
    main_screen = lv_obj_create(NULL);

    // --- Main Content Container (Horizontal Layout) ---
    lv_obj_t * main_content_cont = lv_obj_create(main_screen);
    lv_obj_set_size(main_content_cont, LV_PCT(100), LV_PCT(100)); // Full screen
    lv_obj_set_flex_flow(main_content_cont, LV_FLEX_FLOW_ROW); // Horizontal layout
    lv_obj_set_flex_align(main_content_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // Align children
    lv_obj_set_style_border_width(main_content_cont, 0, 0); // Remove border
    lv_obj_set_style_pad_all(main_content_cont, 0, 0); // Remove padding
    // Allow only vertical scrolling if content overflows (though unlikely full screen)
    lv_obj_set_scroll_dir(main_content_cont, LV_DIR_VER);

    // --- Left Container for Buttons (50% width) ---
    lv_obj_t * left_cont = lv_obj_create(main_content_cont);
    lv_obj_set_size(left_cont, LV_PCT(50), LV_PCT(100)); // 50% width, full height
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN); // Vertical layout
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // Center buttons
    lv_obj_set_style_border_width(left_cont, 0, 0); // Remove border
    lv_obj_set_style_pad_all(left_cont, 10, 0); // Add some padding

    // --- Right Container for Information (50% width) ---
    lv_obj_t * right_cont = lv_obj_create(main_content_cont);
    lv_obj_set_size(right_cont, LV_PCT(50), LV_PCT(100)); // 50% width, full height
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN); // Vertical layout
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // Center content
    lv_obj_set_style_border_width(right_cont, 0, 0); // Remove border
    lv_obj_set_style_pad_all(right_cont, 10, 0); // Add some padding

    // --- Create Buttons in Left Container ---
    lv_obj_t * buttons[4];
    const char * button_labels[] = {"Go to Screen 1", "Go to Screen 2", "Go to Screen 3", "Go to Screen 4"};

    for (int i = 0; i < 4; i++) {
        buttons[i] = lv_button_create(left_cont);
        lv_obj_t * label = lv_label_create(buttons[i]);
        lv_label_set_text(label, button_labels[i]);
        lv_obj_center(label);
        lv_obj_add_event_cb(buttons[i], main_menu_event_handler, LV_EVENT_CLICKED, (void*)(i + 1)); // Pass screen ID 1-4
    }

    // --- Create Information Labels in Right Container ---
    // Time Label
    time_label = lv_label_create(right_cont);
    lv_label_set_text(time_label, "00:00:00"); // Initial text
    // Optional: Style for time
    // lv_obj_set_style_text_font(time_label, &lv_font_montserrat_20, 0);

    // Date Label
    date_label = lv_label_create(right_cont);
    lv_label_set_text(date_label, "1970-01-01"); // Initial text

    // Weather Label
    weather_label = lv_label_create(right_cont);
    lv_label_set_text(weather_label, "Sunny"); // Initial text

    // Welcome Label
    welcome_label = lv_label_create(right_cont);
    lv_label_set_text(welcome_label, "Welcome!"); // Initial text
    // Start animation on the welcome label
    start_welcome_animation(welcome_label);

    // --- Example of updating information (call this elsewhere when data changes) ---
    // Simulate initial data update 更新函数
    update_main_screen_info("14:30:00", "2023-10-27", "Partly Cloudy", "Hello, User!");
}

extern lv_obj_t * screen_1;
extern void back_to_main_event_handler(lv_event_t * e);

void create_screen_1(void) {
    // --- Basic Screen Setup ---
    screen_1 = lv_obj_create(NULL);

    // Add a title label
    lv_obj_t * title = lv_label_create(screen_1);
    lv_label_set_text(title, "Data Visualization Screen");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Add a back button
    lv_obj_t * back_btn = lv_button_create(screen_1);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back to Main Menu");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL);

    // --- Data Visualization Content ---
    // Example data array (values between 0 and 100 for bar visualization)
    int data_values[10] = {25, 78, 45, 90, 15, 63, 38, 82, 55, 70};

    // Create a container for the list of data items
    lv_obj_t * list_cont = lv_obj_create(screen_1);
    lv_obj_set_size(list_cont, LV_PCT(90), LV_VER_RES - 80); // Adjust height as needed
    lv_obj_align_to(list_cont, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(list_cont, LV_SCROLLBAR_MODE_AUTO);

    // --- Styles for the bars ---
    // Style for the background/main part of the bar
    static lv_style_t bar_bg_style;
    lv_style_init(&bar_bg_style);
    lv_style_set_radius(&bar_bg_style, 4); // Rounded corners
    lv_style_set_bg_color(&bar_bg_style, lv_color_hex(0xDDDDDD)); // Light gray background
    lv_style_set_bg_opa(&bar_bg_style, LV_OPA_COVER);
    lv_style_set_border_color(&bar_bg_style, lv_color_hex(0xAAAAAA)); // Gray border
    lv_style_set_border_width(&bar_bg_style, 1);
    lv_style_set_pad_all(&bar_bg_style, 2); // Some padding inside the bar background

    // Style for the indicator part of the bar (the filled portion)
    static lv_style_t bar_indic_style;
    lv_style_init(&bar_indic_style);
    lv_style_set_radius(&bar_indic_style, 3); // Slightly smaller radius
    // You can change this color for higher contrast or to represent different meanings
    lv_style_set_bg_color(&bar_indic_style, lv_color_hex(0x007BFF)); // Vivid Blue
    // lv_style_set_bg_color(&bar_indic_style, lv_color_hex(0x28A745)); // Or Vivid Green
    // lv_style_set_bg_color(&bar_indic_style, lv_color_hex(0xDC3545)); // Or Vivid Red
    lv_style_set_bg_opa(&bar_indic_style, LV_OPA_COVER);

    // Create 10 data items
    for (int i = 0; i < 10; i++) {
        // Container for a single data item (label, value, bar)
        lv_obj_t * item_cont = lv_obj_create(list_cont);
        lv_obj_set_size(item_cont, LV_PCT(100), 40); // Full width, fixed height
        lv_obj_set_flex_flow(item_cont, LV_FLEX_FLOW_ROW); // Horizontal layout
        lv_obj_set_flex_align(item_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_clear_flag(item_cont, LV_OBJ_FLAG_SCROLLABLE);
          
        // Label for data name
        char label_text[16];
        snprintf(label_text, sizeof(label_text), "Data %d:", i);
        lv_obj_t * label = lv_label_create(item_cont);
        lv_label_set_text(label, label_text);
        lv_obj_set_width(label, 60); // Set a fixed width for labels

        // Label for data value
        char value_text[8];
        snprintf(value_text, sizeof(value_text), "%d", data_values[i]);
        lv_obj_t * value_label = lv_label_create(item_cont);
        lv_label_set_text(value_label, value_text);
        lv_obj_set_width(value_label, 40); // Set a fixed width for values

        // Bar for data visualization
        lv_obj_t * bar = lv_bar_create(item_cont);
        lv_bar_set_range(bar, 0, 100); // Set range 0-100
        lv_bar_set_value(bar, data_values[i], LV_ANIM_OFF); // Set current value
        lv_obj_set_flex_grow(bar, 1); // Bar takes remaining space
        // Apply styles: background/main style to PART_MAIN, indicator style to PART_INDICATOR
        lv_obj_add_style(bar, &bar_bg_style, LV_PART_MAIN);
        lv_obj_add_style(bar, &bar_indic_style, LV_PART_INDICATOR);
        // Optional: Set a specific height for the bar
        lv_obj_set_height(bar, 20);

        // Make the bar non-interactive to prevent scrolling conflicts
        lv_obj_remove_event_cb(bar, NULL);
    }
}

// Assuming these are declared globally or passed appropriately
extern lv_obj_t * screen_2;
extern void back_to_main_event_handler(lv_event_t * e);

// --- Forward declarations for event handlers ---
static void button_1_event_handler(lv_event_t * e);
static void button_2_event_handler(lv_event_t * e);
static void button_3_event_handler(lv_event_t * e);
static void button_4_event_handler(lv_event_t * e);

static void switch_1_event_handler(lv_event_t * e);
static void switch_2_event_handler(lv_event_t * e);
static void switch_3_event_handler(lv_event_t * e);
static void switch_4_event_handler(lv_event_t * e);

// --- Event Handler Definitions (Empty Bodies) ---
static void button_1_event_handler(lv_event_t * e) {
    // Event handling logic for Button 1 goes here
    // LV_UNUSED(e); // Use if 'e' is not used to suppress compiler warnings
    // printf("Button 1 Clicked\n");
}

static void button_2_event_handler(lv_event_t * e) {
    // Event handling logic for Button 2 goes here
}

static void button_3_event_handler(lv_event_t * e) {
    // Event handling logic for Button 3 goes here
}

static void button_4_event_handler(lv_event_t * e) {
    // Event handling logic for Button 4 goes here
}

static void switch_1_event_handler(lv_event_t * e) {
    // Event handling logic for Switch 1 goes here
    // You can get the state using:
    // lv_obj_t * sw = lv_event_get_target_obj(e);
    // bool is_on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    // printf("Switch 1 Toggled: %s\n", is_on ? "ON" : "OFF");
}

static void switch_2_event_handler(lv_event_t * e) {
    // Event handling logic for Switch 2 goes here
}

static void switch_3_event_handler(lv_event_t * e) {
    // Event handling logic for Switch 3 goes here
}

static void switch_4_event_handler(lv_event_t * e) {
    // Event handling logic for Switch 4 goes here
}


// --- Screen Creation Function ---
void create_screen_2(void) {
    screen_2 = lv_obj_create(NULL);

    // Add a title label
    lv_obj_t * title = lv_label_create(screen_2);
    lv_label_set_text(title, "Buttons & Switches");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5); // Positioned at top center

    // Add a back button
    lv_obj_t * back_btn = lv_button_create(screen_2);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back to Main Menu");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL);

    // --- Main Content Container ---
    // This container holds the left (buttons) and right (switches) sections
    lv_obj_t * main_cont = lv_obj_create(screen_2);
    lv_obj_set_scroll_dir(main_cont, LV_DIR_VER);
    // Position below title and above back button
    lv_obj_set_size(main_cont, LV_PCT(90), LV_VER_RES - 80);
    lv_obj_align_to(main_cont, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    // Set horizontal layout for left/right sections
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // Remove default border/padding for main container if desired
    lv_obj_set_style_border_width(main_cont, 0, 0);
    lv_obj_set_style_pad_all(main_cont, 0, 0);


    // --- Left Container for Buttons ---
    lv_obj_t * left_cont = lv_obj_create(main_cont);
    lv_obj_set_size(left_cont, LV_PCT(50), LV_PCT(100)); // 50% width of main_cont
    // Set vertical layout for buttons
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // Remove default border/padding for left container if desired
    lv_obj_set_style_border_width(left_cont, 0, 0);
    lv_obj_set_style_pad_all(left_cont, 0, 0);

    // --- Right Container for Switches ---
    lv_obj_t * right_cont = lv_obj_create(main_cont);
    lv_obj_set_size(right_cont, LV_PCT(50), LV_PCT(100)); // 50% width of main_cont
    // Set vertical layout for switches
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
     // Remove default border/padding for right container if desired
    lv_obj_set_style_border_width(right_cont, 0, 0);
    lv_obj_set_style_pad_all(right_cont, 0, 0);

    // --- Populate Left Container with Buttons ---
    // Using an array for easier management and potential future styling
    lv_obj_t * buttons[4];
    for (int i = 0; i < 4; i++) {
        buttons[i] = lv_button_create(left_cont);
        lv_obj_t * label = lv_label_create(buttons[i]);
        char btn_text[20];
        snprintf(btn_text, sizeof(btn_text), "Button %d", i + 1);
        lv_label_set_text(label, btn_text);
        lv_obj_center(label);
    }
    // Add event callbacks
    lv_obj_add_event_cb(buttons[0], button_1_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buttons[1], button_2_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buttons[2], button_3_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(buttons[3], button_4_event_handler, LV_EVENT_CLICKED, NULL);

// --- Populate Right Container with Switches and Labels ---
    // Array to hold switch pointers for event callbacks
    lv_obj_t * switches[4];
    // Array of label texts (4 Chinese characters each)
    const char * switch_labels[] = {"Switch1", "Switch2", "Switch3", "Switch4"};

    for (int i = 0; i < 4; i++) {
        // Create a container for each label + switch pair
        lv_obj_t * switch_item_cont = lv_obj_create(right_cont);
        lv_obj_set_size(switch_item_cont, LV_PCT(100), LV_SIZE_CONTENT); // Width 100% of right_cont, height fits content
        lv_obj_set_flex_flow(switch_item_cont, LV_FLEX_FLOW_ROW); // Horizontal layout
        lv_obj_set_flex_align(switch_item_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // Align items to start, center vertically
        lv_obj_set_style_border_width(switch_item_cont, 0, 0); // Optional: remove border
        lv_obj_set_style_pad_all(switch_item_cont, 2, 0); // Optional: add some padding

        // Create the label inside the item container
        lv_obj_t * label = lv_label_create(switch_item_cont);
        lv_label_set_text(label, switch_labels[i]); // Set the text from the array
        lv_obj_set_width(label, 60); // Set fixed width for 4 Chinese characters (adjust if needed)

        // Create the switch inside the item container
        switches[i] = lv_switch_create(switch_item_cont);
        // Optional: Add some margin/padding between label and switch if needed
        // lv_obj_set_style_margin_left(switches[i], 5, 0);
    }
    // Add event callbacks
    lv_obj_add_event_cb(switches[0], switch_1_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(switches[1], switch_2_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(switches[2], switch_3_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(switches[3], switch_4_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

}




// Function to create Screen 3
void create_screen_3(void) {
    screen_3 = lv_obj_create(NULL);

    // Add a title label
    lv_obj_t * title = lv_label_create(screen_3);
    lv_label_set_text(title, "This is Screen 3");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Add a back button
    lv_obj_t * back_btn = lv_button_create(screen_3);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back to Main Menu");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL);
    
    // --- Add content specific to Screen 3 here ---
    // Example: A slider
    lv_obj_t * slider = lv_slider_create(screen_3);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
    lv_obj_center(slider); // Center the slider on the screen
}

// Function to create Screen 4
void create_screen_4(void) {
    screen_4 = lv_obj_create(NULL);

    // Add a title label
    lv_obj_t * title = lv_label_create(screen_4);
    lv_label_set_text(title, "This is Screen 4");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Add a back button
    lv_obj_t * back_btn = lv_button_create(screen_4);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back to Main Menu");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_to_main_event_handler, LV_EVENT_CLICKED, NULL);
    
    // --- Add content specific to Screen 4 here ---
    // Example: A list
    lv_obj_t * list = lv_list_create(screen_4);
    lv_obj_set_size(list, 200, 150);
    lv_obj_center(list); // Center the list on the screen

    lv_list_add_text(list, "Options");
    lv_list_add_button(list, NULL, "Item 1");
    lv_list_add_button(list, NULL, "Item 2");
    lv_list_add_button(list, NULL, "Item 3");
}

// Event handler for buttons on the main menu
static void main_menu_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);

    if(code == LV_EVENT_CLICKED) {
        // Get the user data (screen ID) passed when the event callback was added
        int screen_id = (int)(uintptr_t)lv_event_get_user_data(e); 
        
        switch(screen_id) {
            case 1:
                lv_screen_load(screen_1); // Load Screen 1
                break;
            case 2:
                lv_screen_load(screen_2); // Load Screen 2
                break;
            case 3:
                lv_screen_load(screen_3); // Load Screen 3
                break;
            case 4:
                lv_screen_load(screen_4); // Load Screen 4
                break;
            default:
                break;
        }
    }
}

// Event handler for the "Back" buttons on sub-screens
static void back_to_main_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);

    if(code == LV_EVENT_CLICKED) {
        lv_screen_load(main_screen); // Load the main menu screen
    }
}

// Main function to set up the UI
void setup_ui(void) {
    // Create all screens
    create_main_screen();
    create_screen_1();
    create_screen_2();
    create_screen_3();
    create_screen_4();

    // Load the main screen to display it first
    lv_screen_load(main_screen);
}