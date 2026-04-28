#include "test.h"
#include "console.h"
#include "memory.h"
#include "fs.h"
#include "timer.h"

static int tests_passed = 0;
static int tests_failed = 0;

static void print_line(void) {
    console_write("--------------------------------\n");
}

static void print_test_start(const char* name) {
    print_line();
    console_write("[TEST] ");
    console_write(name);
    console_write("\n");
}

static void print_result(int passed, u32 start_ticks) {
    u32 elapsed = timer_get_ticks() - start_ticks;

    console_write("Time: ");
    console_write_dec(elapsed);
    console_write(" ticks\n");

    if (passed) {
        tests_passed++;
        console_set_color(0x0A);
        console_write("Result: PASS\n");
    } else {
        tests_failed++;
        console_set_color(0x0C);
        console_write("Result: FAIL\n");
    }

    console_set_color(0x07);
}

static void test_pause(void) {
    console_write("\n--- next test ---\n");

    for (volatile u32 i = 0; i < 900000000; i++) {
        // just a delay :/
    }
}

static void test_memory_alloc(void) {
    u32 start = timer_get_ticks();

    print_test_start("memory allocation");

    u32 before = memory_get_heap_used();

    void* a = kmalloc(32);
    void* b = kmalloc(64);

    u32 after = memory_get_heap_used();

    console_write("Heap used before: ");
    console_write_dec(before);
    console_write(" bytes\n");

    console_write("Allocation A: 32 bytes at ");
    console_write_hex((u32)a);
    console_write("\n");

    console_write("Allocation B: 64 bytes at ");
    console_write_hex((u32)b);
    console_write("\n");

    console_write("Heap used after: ");
    console_write_dec(after);
    console_write(" bytes\n");

    int passed = (a != NULL) && (b != NULL) && ((u32)b > (u32)a) && (after > before);

    print_result(passed, start);
}

static void test_memory_write_read(void) {
    u32 start = timer_get_ticks();

    print_test_start("memory write/read");

    char* block = (char*)kmalloc(16);

    if (block == NULL) {
        console_write("Allocation failed.\n");
        print_result(0, start);
        return;
    }

    block[0] = 'O';
    block[1] = 'K';
    block[2] = '\0';

    console_write("Allocated block at: ");
    console_write_hex((u32)block);
    console_write("\n");

    console_write("Stored string: ");
    console_write(block);
    console_write("\n");

    int passed = block[0] == 'O' && block[1] == 'K' && block[2] == '\0';

    print_result(passed, start);
}

static void test_fs_create(void) {
    u32 start = timer_get_ticks();

    print_test_start("file create");

    fs_delete("test.txt");

    int before_count = fs_count();
    int created = fs_create("test.txt");
    int after_count = fs_count();
    int exists = fs_exists("test.txt");

    console_write("File name: test.txt\n");

    console_write("File count before: ");
    console_write_dec(before_count);
    console_write("\n");

    console_write("File count after: ");
    console_write_dec(after_count);
    console_write("\n");

    console_write("Exists after create: ");
    console_write(exists ? "yes\n" : "no\n");

    print_result(created && exists && after_count == before_count + 1, start);
}

static void test_fs_write_read(void) {
    u32 start = timer_get_ticks();

    print_test_start("file write/read");

    fs_delete("data.txt");
    fs_create("data.txt");

    int written = fs_write("data.txt", "hello");
    const char* data = fs_read("data.txt");

    console_write("File name: data.txt\n");
    console_write("Written text: hello\n");

    console_write("Read text: ");
    if (data) {
        console_write(data);
    } else {
        console_write("(null)");
    }
    console_write("\n");

    int passed = written &&
                 data != NULL &&
                 data[0] == 'h' &&
                 data[1] == 'e' &&
                 data[2] == 'l' &&
                 data[3] == 'l' &&
                 data[4] == 'o' &&
                 data[5] == '\0';

    print_result(passed, start);
}

static void test_fs_delete(void) {
    u32 start = timer_get_ticks();

    print_test_start("file delete");

    fs_delete("delete.txt");
    fs_create("delete.txt");

    int before_count = fs_count();
    int deleted = fs_delete("delete.txt");
    int after_count = fs_count();
    int exists = fs_exists("delete.txt");

    console_write("File name: delete.txt\n");

    console_write("File count before delete: ");
    console_write_dec(before_count);
    console_write("\n");

    console_write("File count after delete: ");
    console_write_dec(after_count);
    console_write("\n");

    console_write("Exists after delete: ");
    console_write(exists ? "yes\n" : "no\n");

    print_result(deleted && !exists && after_count == before_count - 1, start);
}

static void test_fs_duplicate_create(void) {
    u32 start = timer_get_ticks();

    print_test_start("duplicate file rejection");

    fs_delete("dup.txt");

    int first = fs_create("dup.txt");
    int second = fs_create("dup.txt");
    int count = fs_count();

    console_write("File name: dup.txt\n");
    console_write("First create: ");
    console_write(first ? "success\n" : "failed\n");

    console_write("Second create: ");
    console_write(second ? "success\n" : "failed\n");

    console_write("Current file count: ");
    console_write_dec(count);
    console_write("\n");

    print_result(first && !second, start);
}

void tests_run_all(void) {
    tests_passed = 0;
    tests_failed = 0;

    u32 total_start = timer_get_ticks();

    console_write("\nBlebaOS Test Runner\n");

test_memory_alloc();
test_pause();

test_memory_write_read();
test_pause();

test_fs_create();
test_pause();

test_fs_write_read();
test_pause();

test_fs_delete();
test_pause();

test_fs_duplicate_create();

    print_line();
    console_write("Test summary\n");

    console_write("Passed: ");
    console_write_dec(tests_passed);
    console_write("\n");

    console_write("Failed: ");
    console_write_dec(tests_failed);
    console_write("\n");

    console_write("Total tests: ");
    console_write_dec(tests_passed + tests_failed);
    console_write("\n");

    console_write("Total time: ");
    console_write_dec(timer_get_ticks() - total_start);
    console_write(" ticks\n");

    print_line();
}