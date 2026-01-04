

# children/base.py

from utils.utils import to_snake_case



class ChildEmitter:
    type_name = None  # "UI_CHILD_LABEL", etc.

    # --- job system ---
    def emit_job_struct_def(self, screen):
        return None

    def emit_job_callback(self, child, screen):
        return None

    def emit_setter_prototype(self, child, screen):
        return None

    def emit_setter(self, child, screen):
        return None

    # --- init ---
    def emit_init_case(self, screen):
        """
        Returns a full 'case UI_CHILD_X:' block
        """
        return None

    # --- child struct ---
    def emit_child_initializer(self, child, index):
        return None
        
    
    
