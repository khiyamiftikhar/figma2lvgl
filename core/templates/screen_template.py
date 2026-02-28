#include "ui_{screen_name}.h"
#include "ui_defs.h"
#include "assets.h"
#include "ui_worker.h"
#include <stdio.h>

ui_screen_t {screen_var} = {{
    .name = "{screen_name}",
    .child_count = {child_count},
    .children = {{
        {struct_entries}
    }},
    .lv_screen = NULL
}};

{callbacks}

{setters}

void ui_{screen_name}_init(void)
{{
    {screen_var}.lv_screen = lv_obj_create(NULL);

    for (int i = 0; i < {child_count}; i++)
    {{
        ui_child_t* c = &{screen_var}.children[i];

        switch (c->type)
        {{
            {inits}
        }}
    }}

    ui_worker_init();
}}