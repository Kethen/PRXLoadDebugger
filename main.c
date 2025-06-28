#include <pspsdk.h>
#include <pspthreadman.h>
#include <pspiofilemgr.h>

#include <string.h>

#include "common.h"
#include "logging.h"

PSP_MODULE_INFO(MODULE_NAME, PSP_MODULE_KERNEL, 1, 0);

static void dump_module_info(){
	SceUID module_ids[128] = {0};
	int num_module_ids = 0;	

	LOG("dumping module info\n");

	int module_id_list_get_status = sceKernelGetModuleIdList(module_ids, sizeof(module_ids), &num_module_ids);
	if(sceKernelGetModuleIdList < 0){
		LOG("failed fetching module id list, 0x%x\n", module_id_list_get_status);
		return;
	}

	for(int i = 0;i < num_module_ids;i++){
		SceKernelModuleInfo module_info = {0};
		module_info.size = sizeof(SceKernelModuleInfo);
		int module_info_get_status = sceKernelQueryModuleInfo(module_ids[i], &module_info);
		if(module_info_get_status != 0){
			LOG("%s: failed fetching module info for module %d, 0x%x\n", __func__, module_ids[i], module_info_get_status);
			continue;
		}

		char name_buf[28] = {0};
		memcpy(name_buf, module_info.name, 27);

		LOG("%s: %d %s\n", __func__, i, name_buf);

		#if 0
		LOG("module #%d %d:\n"
			"SceKernelModuleInfo:\n"
			" size :%d\n"
			" nsegment: %d\n"
			" reserved: 0x%x 0x%x 0x%x\n"
			" segmentaddr: 0x%x 0x%x 0x%x 0x%x\n"
			" segmentsize: 0x%x 0x%x 0x%x 0x%x\n"
			" entry_addr: 0x%x\n"
			" gp_value: 0x%x\n"
			" text_addr: 0x%x\n"
			" text_size: 0x%x\n"
			" data_size: 0x%x\n"
			" bss_size: 0x%x\n"
			" attribute: 0x%x\n"
			" version: 0x%x 0x%x\n"
			" name: %s\n",
			i, module_ids[i],
			module_info.size,
			module_info.nsegment,
			module_info.reserved[0], module_info.reserved[1], module_info.reserved[2],
			module_info.segmentaddr[0], module_info.segmentaddr[1], module_info.segmentaddr[2], module_info.segmentaddr[3],
			module_info.segmentsize[0], module_info.segmentsize[1], module_info.segmentsize[2], module_info.segmentsize[3],
			module_info.entry_addr,
			module_info.gp_value,
			module_info.text_addr,
			module_info.text_size,
			module_info.data_size,
			module_info.bss_size,
			module_info.attribute,
			module_info.version[0], module_info.version[1],
			name_buf
		);

		#if 1
		SceModule2 *module = (SceModule2 *)sceKernelFindModuleByUID(module_ids[i]);
		if(module == NULL){
			LOG("cannot fetch module for %d %s\n", module_ids[i], name_buf);
		}else{
			memcpy(name_buf, module->modname, 27);
			LOG(
				"SceModule2:\n"
				" attribute: 0x%x\n"
				" version: 0x%x 0x%x\n"
				" modname: %s\n"
				" terminal: 0x%x\n"
				" unknown1: 0x%x\n"
				" unknown2: 0x%x\n"
				" modid: %d\n"
				" unknown3: 0x%x 0x%x\n"
				" mpid_text: 0x%x\n"
				" mpid_data: 0x%x\n"
				" ent_top: 0x%x\n"
				" ent_size: 0x%x\n"
				" stub_top: 0x%x\n"
				" stub_size: 0x%x\n"
				" unknown4: 0x%x 0x%x 0x%x 0x%x 0x%x\n"
				" entry_addr: 0x%x\n"
				" gp_value: 0x%x\n"
				" text_addr: 0x%x\n"
				" text_size: 0x%x\n"
				" data_size: 0x%x\n"
				" bss_size: 0x%x\n"
				" nsegment: %d\n"
				" segmentaddr: 0x%x 0x%x 0x%x 0x%x\n"
				" segmentsize: 0x%x 0x%x 0x%x 0x%x\n",
				module->attribute,
				module->version[0], module->version[1],
				name_buf,
				module->terminal,
				module->unknown1,
				module->unknown2,
				module->modid,
				module->unknown3[0], module->unknown3[1],
				(unsigned int)module->mpid_text,
				(unsigned int)module->mpid_data,
				(unsigned int)module->ent_top,
				module->ent_size,
				(unsigned int)module->stub_top,
				module->stub_size,
				module->unknown4[0], module->unknown4[1], module->unknown4[2], module->unknown4[3], module->unknown4[4],
				module->entry_addr,
				module->gp_value,
				module->text_addr,
				module->text_size,
				module->data_size,
				module->bss_size,
				module->nsegment,
				module->segmentaddr[0], module->segmentaddr[1], module->segmentaddr[2], module->segmentaddr[3],
				module->segmentsize[0], module->segmentsize[1], module->segmentsize[2], module->segmentsize[3]
			);
		}
		#endif
		#endif
	}
}


int loader_thread(SceSize args, void *argp){
	LOG("%s: begin\n", __func__);

	dump_module_info();

	char modpath[128] = {0};
	sprintf(modpath, "ms0:/seplugins/%s", MODULE_NAME "_target.prx");
	int testfd = sceIoOpen(modpath, PSP_O_RDONLY, 0777);
	if (testfd < 0){
		LOG("%s: failed opening %s\n", __func__, modpath);
		sprintf(modpath, "ef0:/seplugins/%s", MODULE_NAME "_target.prx");
		if (testfd < 0){
			LOG("%s: failed opening %s\n", __func__, modpath);
			return 0;
		}
	}

	sceIoClose(testfd);

	int load_status = sceKernelLoadModule(modpath, 0, NULL);
	if (load_status < 0){
		LOG("%s: failed loading %s, 0x%x\n", __func__, modpath, load_status);
		return 0;
	}

	int status;
	int start_status = sceKernelStartModule(load_status, 0, NULL, &status, NULL);

	if (start_status < 0){
		LOG("%s: failed starting %s, 0x%x\n", __func__, modpath, start_status);
		return 0;
	}
	LOG("%s: module %s loaded and started\n", __func__, modpath);

	dump_module_info();

	return 0;
}

int module_start(int args, void *argp){
	LOG("%s: begin\n", __func__);

	// We cannot load modules here due to 0x80020143, so we need to spawn a thread to do that
	SceUID tid = sceKernelCreateThread(MODULE_NAME, loader_thread, 0x18, 0x1000, 0, NULL);
	if (tid < 0){
		LOG("%s: failed creating loader thread, 0x%x\n", __func__, tid);
		return 0;
	}

	sceKernelStartThread(tid, 0, NULL);

	return 0;
}

int module_stop(int args, void *argp){
	LOG("%s: begin\n", __func__);
	return 0;
}
