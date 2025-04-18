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
    plt.rcParams["font.family"] = "monospace"
    fig, ax = plt.subplots()
    fig.subplots_adjust(left=0.99, right=1, bottom=0.99, top=1)
    
    bbox = fig.get_window_extent().transformed(fig.dpi_scale_trans.inverted())
    canvas_width, canvas_height = bbox.width*fig.dpi, bbox.height*fig.dpi

    return_value: Ptr = Ptr()
    return_value.set(None)
    
    buttons_axes: list[plt.Axes] = []
    buttons: list[ButtonProcessor] = []
    
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
    
    def buttons_callback(value: str):
        if (return_value.get() == value):
            plt.close(fig)
        else:
            for b in buttons:
                if b.parameter == value:
                    b.button.color = "#3a79de"
                    b.button.hovercolor = "#4287f5"
                else:
                    b.button.color = "0.85"
                    b.button.hovercolor = "0.95"
            return_value.set(value)
            
            file_content: str = ""
            with open(dir_path + "\\data\\" + value) as f: file_content = f.read().split("\n/*scores*/\n")[0]
            text.set_text(file_content)
            text_slider_factor.set(file_content.count("\n") * 0.1)
            text_slider.set_val(1)
            
            fig.canvas.draw_idle()
    def text_update(event):
        text.set_position((0.55, 0.9 + (text_slider.valmax - text_slider.val) * text_slider_factor.get() * 0.1))
        fig.canvas.draw_idle()
    text_slider.on_changed(text_update)  
    
    for f in all_data_files:
        buttons_axes.append(fig.add_axes([0.05, 0, 0.4, 0.18]))
        buttons.append(ButtonProcessor(buttons_axes[-1], f.removesuffix(".rundata"), buttons_callback, f))

    ax_slider = fig.add_axes([0.005, 0.1, 0.05, 0.8])
    slider = Slider(
        ax=ax_slider,
        label="",
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
        if (event.x + event.x < canvas_width):
            slider.set_val(max(slider.valmin, min(slider.valmax, slider.val + increment * 0.5)))
        else:
            text_slider.set_val(max(text_slider.valmin, min(text_slider.valmax, text_slider.val + increment * 0.5 / max(text_slider_factor.get(), 0.01))))
        update(None)

    fig.canvas.mpl_connect('scroll_event', on_scroll)

    plt.show()
    return dir_path + "\\data\\" + return_value.get()


if __name__ == "__main__":
    file_selector()