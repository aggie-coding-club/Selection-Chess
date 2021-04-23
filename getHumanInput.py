# Echos text box input to program output, then closes

import tkinter as tk

def evaluate(event):
    print(entry.get())
    global w
    w.destroy()

w = tk.Tk()
tk.Label(w, text="Your Move:").pack()
entry = tk.Entry(w)
entry.bind("<Return>", evaluate)
entry.pack()
res = tk.Label(w)
res.pack()

w.mainloop()
