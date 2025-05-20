import os
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from button import ButtonProcessor, ButtonToggle

BUTTON_HEIGHT: int = 0.1

dir_path = os.path.dirname(os.path.realpath(__file__))

def _sort_key(s: str)->str:
    l: list = os.path.basename(s).split("_")
    return "_".join(l[:-4] + [l[-1], l[-2], l[-3], l[-4]])
class DirInfos:
    current_dir: str
    next_dir: list[str]
    files: list[str]
    def __init__(self, current_dir, next_dir, files):
        self.current_dir: str = current_dir
        self.next_dir: list[str] = next_dir
        self.files: list[str] = files
        files.sort(key=_sort_key)
    def __repr__(self):
        return "(" + self.current_dir + ", " + str(self.next_dir) + ", " + str(self.files) + ")"

files = list(os.walk(dir_path+"\\data"))
files: list[DirInfos] = [DirInfos(f[0].removeprefix(dir_path+"\\data"), f[1], f[2]) for f in files]
files: dict[str, DirInfos] = { d.current_dir : d for d in files }

class Ptr:
    def set(self, value):
        self.value = value
    def get(self):
        return self.value

def file_selector() -> tuple[list[str], str]:
    plt.rcParams["font.family"] = "monospace"
    fig, ax = plt.subplots()
    fig.subplots_adjust(left=0.99, right=1, bottom=0.99, top=1)

    buttons_axes: list[plt.Axes] = []
    buttons: list[ButtonToggle] = []
    
    dir_states: dict[str, bool] = {'': True}
    
    text = fig.text(0.55, 0.9, 'select a file', verticalalignment='top')
    
    ax_text_slider = fig.add_axes([0.505, 0.1, 0.05, 0.8])
    text_slider = Slider(
        ax=ax_text_slider,
        label="",
        valmin=0,
        valmax=1,
        valinit=1,
        orientation="vertical"
    )
    text_slider_factor: Ptr = Ptr()
    text_slider_factor.set(0)
    
    def text_update(event):
        text.set_position((0.55, 0.9 + (text_slider.valmax - text_slider.val) * text_slider_factor.get() * 0.1))
        fig.canvas.draw_idle()
    text_slider.on_changed(text_update)

    ax_slider = fig.add_axes([0.005, 0.1, 0.05, 0.8])
    slider = Slider(
        ax=ax_slider,
        label="",
        valmin=0,
        valmax=1,
        valinit=0,
        orientation="vertical"
    )
    def update(event):
        i = 0
        for index in range(len(buttons_axes)):
            current_dir = buttons[index].parent_dir
            buttons_axes[index].set_visible(dir_states[current_dir])
            if (dir_states[current_dir]):
                pos = buttons_axes[index].get_position()
                pos.y0 = 0.5 - (i - (slider.valmax - slider.val)) * (BUTTON_HEIGHT + 0.01)
                pos.y1 = 0.5 + BUTTON_HEIGHT - (i - (slider.valmax - slider.val)) * (BUTTON_HEIGHT + 0.01)
                pos.x0 = 0.05 + buttons[index].indent * 0.02
                pos.x1 = 0.45 + buttons[index].indent * 0.02
                i += 1
                buttons_axes[index].set_position(pos)
        
        val = slider.val - slider.valmax
        slider.valmax = i - 1 + 0.0001
        slider.val = val + slider.valmax
        slider.ax.set_xlim(0, slider.valmax)
        
        for b in buttons:
            if b.state and b.path == None:
                b.button.color = "#d6ab2b"
                b.button.hovercolor = "#f5c842"
            elif b.state and b.path.startswith("\\evolution"):
                b.button.color = "#5a83c4"
                b.button.hovercolor = "#6491d9"
            elif b.state:
                b.button.color = "#39b83f"
                b.button.hovercolor = "#40cf47"
            else:
                b.button.color = "0.85"
                b.button.hovercolor = "0.95"
        
        fig.canvas.draw_idle()
    slider.on_changed(update)
    
    def update_text(path: str):
        file_content: str = ""
        with open(dir_path + "\\data\\" + path.removeprefix("\\")) as f: file_content = f.read()
        if (path.startswith("\\evolution")):
            file_content = file_content.split("\n/*scores*/\n")[0]
        elif (path.startswith("\\local_search")):
            file_content = "can't open file"
        text.set_text(file_content)
        text_slider_factor.set(file_content.count("\n") * 0.1)
        text_slider.set_val(1)
        update(None)
    
    def switch_dir_update(dir: str):
        dir_states[dir] = not(dir_states[dir])
        if not(dir_states[dir]):
            for key in dir_states.keys():
                if (key.startswith(dir)):
                    dir_states[key] = False
            for b in buttons:
                if (b.parent_dir.startswith(dir)):
                    b.state = False
        update(None)
    def add_buttons(current_dir: str = '', indent: int = 0):
        for dir in files[current_dir].next_dir:
            buttons_axes.append(fig.add_axes([0.05, 0, 0.4, 0.18]))
            buttons.append(ButtonToggle(buttons_axes[-1], dir, switch_dir_update, current_dir + "\\" + dir))
            buttons[-1].parent_dir = current_dir
            buttons[-1].path = None
            buttons[-1].indent = indent
            dir_states[current_dir + "\\" + dir] = False
            add_buttons(current_dir + "\\" + dir, indent + 1)
        for f in files[current_dir].files:
            buttons_axes.append(fig.add_axes([0.05, 0, 0.4, 0.18]))
            buttons.append(ButtonToggle(buttons_axes[-1], f.removesuffix(".rundata"), update_text, current_dir+"\\"+f))
            buttons[-1].parent_dir = current_dir
            buttons[-1].path = current_dir+"\\"+f
            buttons[-1].indent = indent
    add_buttons()
    
    update(None)
    
    def on_scroll(event):
        bbox = fig.get_window_extent().transformed(fig.dpi_scale_trans.inverted())
        canvas_width = bbox.width*fig.dpi
    
        increment = 1 if event.button == 'up' else -1
        if (event.x + event.x < canvas_width):
            slider.set_val(max(slider.valmin, min(slider.valmax, slider.val + increment * 0.5)))
        else:
            text_slider.set_val(max(text_slider.valmin, min(text_slider.valmax, text_slider.val + increment * 0.5 / max(text_slider_factor.get(), 0.01))))
        update(None)
    fig.canvas.mpl_connect('scroll_event', on_scroll)

    submit: Ptr = Ptr()
    submit.set(False)
    def submit_callback(event):
        submit.set(True)
        plt.close(fig)
    submit_axe = fig.add_axes([0.65, 0.05, 0.3, 0.05])
    submit_button = Button(submit_axe, "submit")
    submit_button.on_clicked(submit_callback)

    plt.show()
    
    if (not(submit.get())):
        exit()
    
    data_type: str = ""
    result = []
    for b in buttons:
        if (b.state and b.path != None):
            result.append(dir_path + "\\data\\" + b.path.removeprefix("\\"))
            if (b.path.startswith("\\evolution") and data_type in ["evolution", ""]):
                data_type = "evolution"
            elif (b.path.startswith("\\local_search") and data_type in ["local_search", ""]):
                data_type = "local_search"
            else:
                print("ERROR: please select files of the same type")
                exit(1)
    
    if (len(result) == 0):
        print("ERROR: please select at least one file")
        exit(1)

    return result, data_type

if __name__ == "__main__":
    print(file_selector())