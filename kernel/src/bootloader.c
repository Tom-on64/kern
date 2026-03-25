#include <bootloader.h>
#include <kernel.h>
#include <limine.h>
#include <errno.h>

// This defines our expected Limine version
static volatile uint64_t lim_base_revision[] = LIMINE_BASE_REVISION(4);

// Limine requests
static struct limine_paging_mode_request lim_paging_req =
	{ .id = LIMINE_PAGING_MODE_REQUEST_ID, .revision = 4,
	  .mode = LIMINE_PAGING_MODE_X86_64_4LVL, };

static struct limine_executable_address_request lim_kaddr_req =
	{ .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST_ID, .revision = 4 };

static struct limine_hhdm_request lim_hhdm_req = 
	{ .id = LIMINE_HHDM_REQUEST_ID, .revision = 4 };

static struct limine_memmap_request lim_memmap_req =
	{ .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 4 };

static struct limine_framebuffer_request lim_fbuf_req =
	{ .id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 4 };

static struct limine_executable_file_request lim_exe_req =
	{ .id = LIMINE_EXECUTABLE_FILE_REQUEST_ID, .revision = 4 };

// Bootloader data struct
struct bootloader bootloader = { 0 };

int boot_init(void) {
	if (LIMINE_BASE_REVISION_SUPPORTED(lim_base_revision) == false) panic("Unsupported Limine base revision.");

	struct limine_paging_mode_response* lim_paging_res = lim_paging_req.response;
	struct limine_executable_address_response* lim_kaddr_res = lim_kaddr_req.response;
	struct limine_hhdm_response* lim_hhdm_res = lim_hhdm_req.response;
	struct limine_memmap_response* lim_memmap_res = lim_memmap_req.response;
	struct limine_framebuffer_response* lim_fbuf_res = lim_fbuf_req.response;
	struct limine_executable_file_response* lim_exe_res = lim_exe_req.response;

	if (lim_paging_res->mode != LIMINE_PAGING_MODE_X86_64_4LVL) panic("Expected 4 level paging.");
	bootloader.hhdm_offset = lim_hhdm_res->offset;
	bootloader.kernel_phys_base = lim_kaddr_res->physical_base;
	bootloader.kernel_virt_base = lim_kaddr_res->virtual_base;
	bootloader.mm_entries = lim_memmap_res->entries;
	bootloader.mm_entry_count = lim_memmap_res->entry_count;
	bootloader.executable = lim_exe_res->executable_file;

	bootloader.mm_total_memory = 0;
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		if (entry->type == LIMINE_MEMMAP_USABLE) bootloader.mm_total_memory += entry->length;
	}

	bootloader.fb_entries = lim_fbuf_res->framebuffers;
	bootloader.fb_entry_count = lim_fbuf_res->framebuffer_count;

	return SUCCESS;
}

