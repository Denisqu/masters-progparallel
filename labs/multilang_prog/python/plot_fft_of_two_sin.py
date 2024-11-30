import sys
sys.path.insert(0, '/home/user/univer/homework_and_labs/labs/multilang_prog/python/cxx-shared-lib')
import fft_pylib
import cmath
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button

def is_power_of_two(n):
    return (n > 0) and (n & (n - 1)) == 0

class FFTPlotter:
    def __init__(self, Fs, f1_init, f2_init):
        self.Fs = Fs
        self.T = 1 / Fs
        self.t = np.arange(0, 1.0, self.T)
        self.f1_init = f1_init
        self.f2_init = f2_init
        self.FFTConverterParallel = fft_pylib.FFTConverterParallel

        self.fig, (self.ax1, self.ax2) = plt.subplots(2, 1, figsize=(10, 8))
        plt.subplots_adjust(left=0.1, bottom=0.35, hspace=0.5)

        self.signal1 = np.sin(2 * np.pi * f1_init * self.t)
        self.signal2 = np.sin(2 * np.pi * f2_init * self.t)
        self.sum_signal = self.signal1 + self.signal2
        self.line1, = self.ax1.plot(self.t, self.sum_signal, lw=2)

        self.sum_signal_complex = [cmath.rect(i, 0) for i in self.sum_signal]
        while not is_power_of_two(len(self.sum_signal_complex)):
            self.sum_signal_complex.append(cmath.rect(0, 0))

        self.fft_result = self.FFTConverterParallel.convert_parallel(self.sum_signal_complex)
        self.freqs = np.fft.fftfreq(len(self.sum_signal_complex), self.T)
        self.line2, = self.ax2.plot(self.freqs[:len(self.sum_signal_complex)//2], np.abs(self.fft_result[:len(self.sum_signal_complex)//2]))

        self.ax1.set_title('Sum of Signals')
        self.ax1.set_xlabel('Time [s]')
        self.ax1.set_ylabel('Amplitude')

        self.ax2.set_title('FFT of the Sum of Signals')
        self.ax2.set_xlabel('Frequency [Hz]')
        self.ax2.set_ylabel('Magnitude')

        self.axfreq1 = plt.axes([0.1, 0.2, 0.8, 0.03], facecolor='lightgoldenrodyellow')
        self.freq_slider1 = Slider(
            ax=self.axfreq1,
            label='Frequency 1 [Hz]',
            valmin=1,
            valmax=128,
            valinit=f1_init,
        )

        self.axfreq2 = plt.axes([0.1, 0.15, 0.8, 0.03], facecolor='lightgoldenrodyellow')
        self.freq_slider2 = Slider(
            ax=self.axfreq2,
            label='Frequency 2 [Hz]',
            valmin=1,
            valmax=128,
            valinit=f2_init,
        )

        self.freq_slider1.on_changed(self.update)
        self.freq_slider2.on_changed(self.update)

        self.resetax = plt.axes([0.8, 0.001, 0.1, 0.04])
        self.button = Button(self.resetax, 'Reset', color='lightgoldenrodyellow', hovercolor='0.975')
        self.button.on_clicked(self.reset)

    def update(self, val):
        freq1 = self.freq_slider1.val
        freq2 = self.freq_slider2.val
        signal1 = np.sin(2 * np.pi * freq1 * self.t)
        signal2 = np.sin(2 * np.pi * freq2 * self.t)
        sum_signal = signal1 + signal2
        self.line1.set_ydata(sum_signal)

        sum_signal_complex = [cmath.rect(i, 0) for i in sum_signal]
        while not is_power_of_two(len(sum_signal_complex)):
            sum_signal_complex.append(cmath.rect(0, 0))

        fft_result = self.FFTConverterParallel.convert_parallel(sum_signal_complex)
        self.line2.set_ydata(np.abs(fft_result[:len(sum_signal_complex)//2]))

        self.fig.canvas.draw_idle()

    def reset(self, event):
        self.freq_slider1.reset()
        self.freq_slider2.reset()

if __name__ == "__main__":
    Fs = 256  
    f1_init = 1  
    f2_init = 1  

    plotter = FFTPlotter(Fs, f1_init, f2_init)
    plt.show()
