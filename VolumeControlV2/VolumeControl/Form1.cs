using System;
using AudioSwitcher.AudioApi.CoreAudio;
using System.Windows.Forms;
using ArduCSharp;
using System.Linq;
using WindowsInput.Native;
using WindowsInput;

namespace VolumeControl
{
    public partial class Form1 : Form
    {
        InputSimulator sim = new InputSimulator();

        public Form1()
        {
            InitializeComponent();
        }
        CoreAudioDevice defaultPlaybackDevice = new CoreAudioController().DefaultPlaybackDevice;
        private void Form1_Load(object sender, EventArgs e)
        {
            foreach (var com in Serial.GetSerialsAvailables())
            {
                COM.Items.Add(com);
                COM.SelectedItem = com;
                Connect.Enabled = true;

            }
        }

        private void Connect_Click(object sender, EventArgs e)
        {
            Serial.Open((String)COM.SelectedItem, 1200);
            Serial.PortUseArduino.DataReceived += PortUseArduino_DataReceived;
        }

        private void PortUseArduino_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            try
            {
                Invoke((MethodInvoker)delegate
                {
                    label1.Text = Serial.GetLastData();
                });

                var msg = Serial.GetLastData().Split('!');





                if (msg[1].Contains('>'))
                {
                    sim.Keyboard.KeyPress(VirtualKeyCode.RIGHT);
                    Invoke((MethodInvoker)delegate
                    {
                        Connect.Text = "Next";
                    });

                    return;

                }
                else if (msg[1].Contains('<'))
                {
                    Invoke((MethodInvoker)delegate
                    {
                        Connect.Text = "Prev";
                    });
                    sim.Keyboard.KeyPress(VirtualKeyCode.LEFT);
                    return;
                }
                else if (msg[1].Contains('-'))
                {
                    Invoke((MethodInvoker)delegate
                    {
                        Connect.Text = "Pause";
                    });
                    sim.Keyboard.KeyPress(VirtualKeyCode.SPACE);
                    return;
                }
                var vol = Convert.ToInt32(msg[1]);

                if (vol <= 0)
                {
                    vol = 0;
                }
                if (vol >= 100)
                {
                    vol = 100;
                }
                defaultPlaybackDevice.Volume = vol;
                Invoke((MethodInvoker)delegate
                {
                    Volume.Value = Convert.ToInt32(vol);
                    Connect.Text = vol.ToString();

                });

            }
            catch { }


        }
    }
}
