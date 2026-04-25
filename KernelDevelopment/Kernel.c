void main() {
    // Pointer to the start of video memory
    char* video_memory = (char*) 0xb8000;

    // Write 'X' to the top-left of the screen
    // 0xb8000 = character ASCII
    // 0xb8001 = attribute (color)
    video_memory[0] = 'B'; 
    video_memory[1] = 0x0F; // White text on black background
    video_memory[2] = 'L';
    video_memory[3] = 0x0F;
    video_memory[4] = 'E';
    video_memory[5] = 0x0F;
    video_memory[6] = 'B';
    video_memory[7] = 0x0F;
    video_memory[8] = 'A';
    video_memory[9] = 0x0F;


    return;
}