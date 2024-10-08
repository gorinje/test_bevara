#include <gpac/filters.h>


typedef struct
{
	u32 opt1;
	Bool opt2;

	GF_FilterPid *src_pid;
	GF_FilterPid *dst_pid;
} GF_BaseFilter;

static void base_filter_finalize(GF_Filter *filter)
{
	//peform any finalyze routine needed, including potential free in the filter context
	//if not needed, set the filter_finalize to NULL


	// inserting test lines


}
static GF_Err base_filter_process(GF_Filter *filter)
{
	u8 *data_dst;
	const u8 *data_src;
	u32 size;

	GF_FilterPacket *pck_dst;
	GF_BaseFilter *stack = (GF_BaseFilter *) gf_filter_get_udta(filter);

	GF_FilterPacket *pck = gf_filter_pid_get_packet(stack->src_pid);
	if (!pck) return GF_OK;
	data_src = gf_filter_pck_get_data(pck, &size);

	//produce output packet using memory allocation
	pck_dst = gf_filter_pck_new_alloc(stack->dst_pid, size, &data_dst);
	if (!pck_dst) return GF_OUT_OF_MEM;
	memcpy(data_dst, data_src, size);

	//no need to adjust data framing
//	gf_filter_pck_set_framing(pck_dst, GF_TRUE, GF_TRUE);

	//copy over src props to dst
	gf_filter_pck_merge_properties(pck, pck_dst);
	gf_filter_pck_send(pck_dst);

	gf_filter_pid_drop_packet(stack->src_pid);
	return GF_OK;

}

static GF_Err base_filter_config_input(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	const GF_PropertyValue *format;
	GF_PropertyValue p;
	GF_BaseFilter  *stack = (GF_BaseFilter *) gf_filter_get_udta(filter);

	if (stack->src_pid==pid) {
		//disconnect of src pid (not yet supported)
		if (is_remove) {
		}
		//update of caps, check everything is fine
		else {
		}
		return GF_OK;
	}
	//check input pid properties we are interested in
	format = gf_filter_pid_get_property(pid, GF_4CC('c','u','s','t') );
	if (!format || !format->value.string || strcmp(format->value.string, "myformat")) {
		return GF_NOT_SUPPORTED;
	}


	//setup output (if we are a filter not a sink)

	stack->dst_pid = gf_filter_pid_new(filter);
	gf_filter_pid_copy_properties(stack->dst_pid, stack->src_pid);
	p.type = GF_PROP_UINT;
	p.value.uint = 10;
	gf_filter_pid_set_property(stack->dst_pid, GF_4CC('c','u','s','2'), &p);

	//set framing mode if needed - by default all PIDs require complete data blocks as inputs
//	gf_filter_pid_set_framing_mode(pidctx->src_pid, GF_TRUE);

	return GF_OK;
}

static GF_Err base_filter_update_arg(GF_Filter *filter, const char *arg_name, const GF_PropertyValue *arg_val)
{
	return GF_OK;
}

GF_Err base_filter_initialize(GF_Filter *filter)
{
	GF_BaseFilter *stack = gf_filter_get_udta(filter);
	if (stack->opt2) {
		//do something based on options

	}
	//if you filter is a source, this is the right place to start declaring output PIDs, such as above

	return GF_OK;
}

#define OFFS(_n)	#_n, offsetof(GF_BaseFilter, _n)
static const GF_FilterArgs ExampleFilterArgs[] =
{
	//example uint option using enum, result parsed ranges from 0(=v1) to 2(=v3)
	{ OFFS(opt1), "Example option 1", GF_PROP_UINT, "v1", "v1|v2|v3", 0},
	{ OFFS(opt2), "Example option 2", GF_PROP_BOOL, "false", NULL, 0},
	{ NULL }
};

static const GF_FilterCapability BaseFilterCaps[] =
{
	CAP_UINT(GF_CAPS_INPUT, GF_PROP_PID_STREAM_TYPE, GF_STREAM_FILE),
	CAP_STRING(GF_CAPS_INPUT, GF_PROP_PID_FILE_EXT, "ext1|ext2|ext3"),
	CAP_STRING(GF_CAPS_INPUT, GF_PROP_PID_MIME, "audio/mimetype1|video/mimetype1"),
	CAP_UINT(GF_CAPS_OUTPUT, GF_PROP_PID_STREAM_TYPE, GF_STREAM_AUDIO),
	CAP_UINT(GF_CAPS_OUTPUT, GF_PROP_PID_STREAM_TYPE, GF_STREAM_VISUAL),
};

const GF_FilterRegister FilterRegister = {
	.name = "Base filter",
	GF_FS_SET_DESCRIPTION("Short description of the filter")
	GF_FS_SET_HELP("Long description of the filter.")
	.private_size = sizeof(GF_BaseFilter),
	.args = ExampleFilterArgs,
	.initialize = base_filter_initialize,
	.finalize = base_filter_finalize,
	SETCAPS(BaseFilterCaps),
	.process = base_filter_process,
	.configure_pid = base_filter_config_input
};

const GF_FilterRegister * EMSCRIPTEN_KEEPALIVE dynCall_filter_register(GF_FilterSession *session)
{
	return &FilterRegister;
}



