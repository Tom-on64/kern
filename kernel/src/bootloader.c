#include <bootloader.h>
#include <kernel.h>
#include <limine.h>

/*
 * This defines our expected Limine version
 */
static volatile LIMINE_BASE_REVISION(4);

struct bootloader bootloader = { 0 };

static struct limine_paging_mode_request lim_paging_req =
	{ .id = LIMINE_PAGING_MODE_REQUEST, .revision = 4,
	  .mode = LIMINE_PAGING_MODE_X86_64_4LVL, };

static struct limine_kernel_address_request lim_kaddr_req =
	{ .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 4 };

static struct limine_hhdm_request lim_hhdm_req = 
	{ .id = LIMINE_HHDM_REQUEST, .revision = 4 };

static struct limine_memmap_request lim_memmap_req =
	{ .id = LIMINE_MEMMAP_REQUEST, .revision = 4 };

static struct limine_framebuffer_request lim_fbuf_req =
	{ .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0 };

int boot_init(void) {
	if (LIMINE_BASE_REVISION_SUPPORTED == false) panic("Unsupported Limine base revision.");

	struct limine_paging_mode_response* lim_paging_res = lim_paging_req.response;
	struct limine_kernel_address_response* lim_kaddr_res = lim_kaddr_req.response;
	struct limine_hhdm_response* lim_hhdm_res = lim_hhdm_req.response;
	struct limine_memmap_response* lim_memmap_res = lim_memmap_req.response;
	struct limine_framebuffer_response* lim_fbuf_res = lim_fbuf_req.response;

	if (lim_paging_res->mode != LIMINE_PAGING_MODE_X86_64_4LVL) return 1;
	bootloader.hhdm_offset = lim_hhdm_res->offset;
	bootloader.kernel_phys_base = lim_kaddr_res->physical_base;
	bootloader.kernel_virt_base = lim_kaddr_res->virtual_base;
	bootloader.mm_entries = lim_memmap_res->entries;
	bootloader.mm_entry_count = lim_memmap_res->entry_count;

	bootloader.mm_total_memory = 0;
	for (size_t i = 0; i < bootloader.mm_entry_count; i++) {
		struct limine_memmap_entry* entry = bootloader.mm_entries[i];
		if (entry->type == LIMINE_MEMMAP_USABLE) bootloader.mm_total_memory += entry->length;
	}

	bootloader.fb_entries = lim_fbuf_res->framebuffers;
	bootloader.fb_entry_count = lim_fbuf_res->framebuffer_count;

	return 0;
}

