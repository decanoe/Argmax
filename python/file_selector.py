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
    
    buttons_axes: list[plt.Axes] = []
    buttons: list[ButtonProcessor] = []
    for f in all_data_files:
        buttons_axes.append(fig.add_axes([0.15, 0, 0.7, 0.18]))
        buttons.append(ButtonProcessor(buttons_axes[-1], f, buttons_callback, f))

    ax_slider = fig.add_axes([0.1, 0.1, 0.05, 0.8])
    slider = Slider(
        ax=ax_slider,
        label="slider",
        valmin=0,
        valmax=len(all_data_files) - 1 + 0.0001,
        valinit=len(all_data_files) - 1,
        orientation="vertical"
    )
    def update(event):
        i = 0
        for a in buttons_axes:
            pos = a.get_position()
            pos.y0 = 0.5 - (slider.val - i) * 0.2
            pos.y1 = 0.68 - (slider.val - i) * 0.2
            i += 1
            a.set_position(pos)
        
        fig.canvas.draw_idle()
    slider.on_changed(update)
    update(None)
    
    def on_scroll(event):
        increment = 1 if event.button == 'up' else -1
        slider.set_val(max(slider.valmin, min(slider.valmax, slider.val + increment * 0.5)))
        update(None)

    fig.canvas.mpl_connect('scroll_event', on_scroll)

    plt.show()
    return dir_path + "\\data\\" + return_value.get()