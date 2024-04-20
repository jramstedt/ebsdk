static struct  test_dispatch_type 
        {
	char *name;
        char *overlay;
	char *text;
	int  (*rtn)();                                       
        } overlay_tests[]={
	"reg",	        "tcisimple",	"tgatest reg  	  ",     dispatch_simple,
	"vram",	 	"tcisimple",	"tgatest vram		  ",     dispatch_simple,
	"int",	 	"tcisimple",	"tgatest int 		  ",     dispatch_simple,
	"plane", 	"tcisimple",	"tgatest plane	  ",     dispatch_simple,
	"pshift",	"tcisimple",	"tgatest pshift  	  ",     dispatch_simple,
	"simz",  	"tciapp",	"tgatest simz    	  ",     dispatch_app,
	"stip",	 	"tciops",	"tgatest stip    	  ",     dispatch_ops,
	"copy",	 	"tciops",	"tgatest copy    	  ",     dispatch_ops,
	"bool",	 	"tciops",	"tgatest bool    	  ",     dispatch_ops,
	"line", 	"tciline",	"tgatest line   	  ",     dispatch_line,
	"pack", 	"tcipack",	"tgatest pack   	  ",     dispatch_pack,
 	"box",	  	"tcipatt",	"tgatest box    	  ",     dispatch_patt,
 	"font",	  	"tcipatt",	"tgatest font   	  ",     dispatch_patt,
	"vdac",	  	"tciramdac",	"tgatest vdac    	  ",     dispatch_ramdac,
 	"patt",	  	"tcipatt",	"tgatest patt   	  ",     dispatch_patt,
        "stero",	"tcipatt",	"tgatest stereo         ",      dispatch_patt,
        "edit",         "tcipatt",      "tgatest edit           ",     dispatch_patt,
        "chair",        "tcipatt",      "tgatest chair          ",     dispatch_patt,
        "init",         "tcipatt",      "tgatest init           ",     dispatch_patt,
        "cnfg",         "tcipatt",      "tgatest cnfg           ",     dispatch_patt,
       };
