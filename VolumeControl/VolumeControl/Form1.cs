using System;
using AudioSwitcher.AudioApi.CoreAudio;
using System.Windows.Forms;
using ArduCSharp;

namespace VolumeControl
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        CoreAudioDevice defaultPlaybackDevice = new CoreAudioController().DefaultPlaybackDevice;
        private void Form1_Load(object sender, EventArgs e)
        {
            foreach(var com in Serial.GetSerialsAvailables())
            {
                COM.Items.Add(com);
                COM.SelectedItem = com;
                Connect.Enabled = true;
            }
        }

        private void Connect_Click(object sender, EventArgs e)
        {
            Serial.Open((String)COM.SelectedItem, 9600);
            Serial.PortUseArduino.DataReceived += PortUseArduino_DataReceived;
        }

        private void PortUseArduino_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            try
            {
                var message = Serial.GetLastData().Split('/');




                var vol = Convert.ToInt32(message[0]);

                if (vol == 2003)
                {
                    return;
                }

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
