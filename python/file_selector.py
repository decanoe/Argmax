import os
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from button import ButtonProcessor

dir_path = os.path.dirname(os.path.realpath(__file__))
all_data_files = [f for f in os.listdir(dir_path+"\\data")]
def _sort_key(s: str)->str:
    l: list = os.path.basename(s).split("_")
    return l[5] + l[4] + l[3] + l[2]
all_data_files.sort(key=_sort_key)
print(all_data_files)

class Ptr:
    def set(self, value):
        self.value = value
    def get(self):
        return self.value

def file_selector() -> str:
    fig, ax = plt.subplots()
    fig.subplots_adjust(left=0.99, right=1, bottom=0.99, top=1)

    return_value: Ptr = Ptr()
    
    def buttons_callback(value: str):
        return_value.set(value)
        plt.close(fig)
    
    buttons_axes: Ptr = Ptr()
    buttons_axes.set([])
    buttons: Ptr = Ptr()
    buttons.set([])

    ax_slider = fig.add_axes([0.1, 0.1, 0.05, 0.8])
    slider = Slider(
        ax=ax_slider,
        label="slider",
        valmin=0,
        valmax=len(all_data_files) - 1,
        valinit=len(all_data_files) - 1,
        orientation="vertical"
    )
    def update(event):
        for b in buttons.get():
            b.delete()
        for a in buttons_axes.get():
            fig.delaxes(a)
        
        temp_axes = []
        temp_buttons = []
        for f in all_data_files:
            temp_axes.append(fig.add_axes([0.15, 0.8 - (slider.val - len(temp_buttons)) * 0.2, 0.7, 0.18]))
            temp_buttons.append(ButtonProcessor(temp_axes[-1], f, buttons_callback, f))
        buttons.set(temp_buttons)
        buttons_axes.set(temp_axes)
        
        fig.canvas.draw_idle()
    slider.on_changed(update)
    update(None)

    plt.show()
    return dir_path + "\\data\\" + return_value.get()