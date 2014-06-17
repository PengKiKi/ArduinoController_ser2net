using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Controls;
using System.Runtime.InteropServices; // DllImport
using AForge.Video.DirectShow;
using AForge;
using AForge.Video;
namespace Robot_Control
{

    public partial class Form1 : Form
    {
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern void Init();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool Connect(StringBuilder host);//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool Connected();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern void cleanup();//NN
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool IsArmed();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool Reconnect();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.R8)]
        public static extern double GetVoltage();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.R8)]
        public static extern double GetTemperature();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GetMotor1Fault();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GetMotor2Fault();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern UInt16 GetMotor1Current();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern UInt16 GetMotor2Current();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U8)]
        public static extern ulong GetLastReportedTimeAlive();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool UnArm();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool Arm();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern void Run();//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool SetSpeed(Int16 speed_m1, Int16 speed_m2);//K
        [DllImport("Robot_Control.dll", CallingConvention=CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool SetBrake(Int16 speed_m1, Int16 speed_m2);//K

        [DllImport("kernel32.dll")] 
        private static extern long GetTickCount64();
        public Form1()
        {
            InitializeComponent();
        }

        System.Timers.Timer ExecuteRun = new System.Timers.Timer();
        System.Timers.Timer UpdateValuesTimer = new System.Timers.Timer();
        System.Timers.Timer MouseUpdater = new System.Timers.Timer();

        private void Form1_Load(object sender, EventArgs e)
        {
            Init();

            this.KeyPreview = true;

            ExecuteRun.Interval = 1;
            ExecuteRun.Elapsed += ExecuteRun_Tick;
            ExecuteRun.Start();

            UpdateValuesTimer.Interval = 250;
            UpdateValuesTimer.Elapsed += UpdateValuesTimer_Tick;
            UpdateValuesTimer.Start();

            MouseUpdater.Interval = 1;
            MouseUpdater.Elapsed += MouseUpdater_Elapsed;
            MouseUpdater.Start();

            progressBar5.Style = ProgressBarStyle.Continuous;
            progressBar6.Style = ProgressBarStyle.Continuous;

            verticalProgressBar1.Minimum = 0;
            verticalProgressBar2.Minimum = 0;
            verticalProgressBar3.Minimum = 0;
            verticalProgressBar4.Minimum = 11000;

            progressBar5.Minimum = 0;
            progressBar6.Minimum = 0;

            verticalProgressBar1.Maximum = 25000;
            verticalProgressBar2.Maximum = 25000;
            verticalProgressBar3.Maximum = 23000;
            verticalProgressBar4.Maximum = 14000;

            progressBar5.Maximum = 1;
            progressBar6.Maximum = 1;

            verticalProgressBar1.Value = 0;
            verticalProgressBar2.Value = 0;
            verticalProgressBar3.Value = 0;
            verticalProgressBar4.Value = 0;

            progressBar5.Value = 1;
            progressBar6.Value = 1;

            progressBar5.ForeColor = Color.Red;
            progressBar6.ForeColor = Color.Red;

            radioButton1.Location = new System.Drawing.Point(220, 220);
            radioButton1.Checked = true;

        }

        private void UpdateValuesTimer_Tick(object sender, EventArgs e)
        {
            if (Connected() == true)
            {
                if (IsArmed() == true)
                {
                    button2.Text = "Arm Status:\r\nARMED\r\n(Click to unarm)";
                    button2.BackColor = Color.Green;
                    SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
                }
                else
                {
                    button2.Text = "Arm Status:\r\nUNARMED\r\n(Click to arm)";
                    button2.BackColor = Color.Red;
                }
                verticalProgressBar1.Value = GetMotor1Current();
                label4.Text = GetMotor1Current().ToString() + " mA";
                if (GetMotor1Current() < 1000)
                {
                    verticalProgressBar1.Color = Color.Lime;
                }
                else if (GetMotor1Current() < 6000)
                {
                    verticalProgressBar1.Color = Color.Green;
                }
                else if (GetMotor1Current() < 9000)
                {
                    verticalProgressBar1.Color = Color.Yellow;
                }
                else if (GetMotor1Current() < 11000)
                {
                    verticalProgressBar1.Color = Color.Orange;
                }
                else
                {
                    verticalProgressBar1.Color = Color.Red;
                }

                verticalProgressBar2.Value = GetMotor2Current();
                label5.Text = GetMotor2Current().ToString() + " mA";
                if (GetMotor2Current() < 1000)
                {
                    verticalProgressBar2.Color = Color.Lime;
                }
                else if (GetMotor2Current() < 6000)
                {
                    verticalProgressBar2.Color = Color.Green;
                }
                else if (GetMotor2Current() < 9000)
                {
                    verticalProgressBar2.Color = Color.Yellow;
                }
                else if (GetMotor2Current() < 11000)
                {
                    verticalProgressBar2.Color = Color.Orange;
                }
                else
                {
                    verticalProgressBar2.Color = Color.Red;
                }

                verticalProgressBar3.Value = (int)((GetTemperature()+30.0)*100.0);
                label6.Text = GetTemperature().ToString("0.00") + " *C";
                if (GetTemperature() < 10)
                {
                    verticalProgressBar3.Color = Color.LightBlue;
                }else
                if (GetTemperature() < 20)
                {
                    verticalProgressBar3.Color = Color.Blue;
                }else
                if (GetTemperature() < 50)
                {
                    verticalProgressBar3.Color = Color.Green;
                }else
                if (GetTemperature() < 65)
                {
                    verticalProgressBar3.Color = Color.Yellow;
                }else
                if (GetTemperature() < 80)
                {
                    verticalProgressBar3.Color = Color.Orange;
                }else
                {
                    verticalProgressBar3.Color = Color.Red;
                }

                verticalProgressBar4.Value = ((int)(GetVoltage()*1000.0));
                label9.Text = GetVoltage().ToString("0.00") + " V";
                if (GetVoltage() > 12.65)
                {
                    verticalProgressBar4.Color = Color.Lime;
                }
                else if (GetVoltage() > 12.45)
                {
                    verticalProgressBar4.Color = Color.DarkGreen;
                }
                else if (GetVoltage() > 12.24)
                {
                    verticalProgressBar4.Color = Color.Yellow;
                }
                else if (GetVoltage() > 12.06)
                {
                    verticalProgressBar4.Color = Color.Orange;
                }
                else
                {
                    verticalProgressBar4.Color = Color.Red;
                }

                if(GetMotor1Fault())
                {
                    progressBar5.ForeColor = Color.Red;
                    label10.BackColor = Color.Red;
                }
                else
                {
                    progressBar5.ForeColor = Color.Green;
                    label10.BackColor = Color.Green;
                }

                if (GetMotor2Fault())
                {
                    progressBar6.ForeColor = Color.Red;
                    label11.BackColor = Color.Red;
                }
                else
                {
                    progressBar6.ForeColor = Color.Green;
                    label11.BackColor = Color.Green;
                }

                label14.Text = (((ulong)GetTickCount64() - GetLastReportedTimeAlive())/1000).ToString();
            }
        }

        private void ExecuteRun_Tick(object sender, EventArgs e)
        {
            Run();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (Connected())
            {
                if (Reconnect())
                {
                    button1.Text = "Connection Status\r\nConnected\r\n(Click to reconnect)";
                    button1.BackColor = Color.Green;
                    //MessageBox.Show("Reconnected succesfully", "Connection status");
                }
                else
                {
                    button1.Text = "Connection Status\r\nDisconnected\r\n(Click to reconnect)";
                    button1.BackColor = Color.Red;
                    //MessageBox.Show("Connection failed", "Connection status");
                }                
            }
            else
            {
                if (Connect(new StringBuilder("192.168.2.222")))
                {
                    button1.Text = "Connection Status\r\nConnected\r\n(Click to reconnect)";
                    button1.BackColor = Color.Green;
                }
                else
                {
                    button1.Text = "Connection Status\r\nDisconnected (failed)\r\n(Click to reconnect)";
                    //MessageBox.Show("Connection failed", "Connection status");
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(IsArmed())
            {
                UnArm();
            }
            else
            {
                Arm();
            }
        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            double hoek = Math.Atan2(trackBar2.Value, trackBar1.Value);
            double distance = Math.Sqrt((trackBar1.Value * trackBar1.Value) + (trackBar2.Value * trackBar1.Value));
            trackBar2.Value = (int)(distance * Math.Sin(-hoek));
            label15.Text = trackBar1.Value.ToString();
            label16.Text = trackBar2.Value.ToString();
            radioButton1.Location = new System.Drawing.Point(trackBar1.Value + 220, 440 - (trackBar2.Value + 220));
        }

        private void trackBar2_Scroll(object sender, EventArgs e)
        {
            double hoek = Math.Atan2(trackBar2.Value, trackBar1.Value);
            double distance = Math.Sqrt((trackBar1.Value * trackBar1.Value) + (trackBar2.Value * trackBar1.Value));
            trackBar1.Value = (int)(distance * Math.Cos(-hoek));
            label15.Text = trackBar1.Value.ToString();
            label16.Text = trackBar2.Value.ToString();
            radioButton1.Location = new System.Drawing.Point(trackBar1.Value + 220, 440 - (trackBar2.Value + 220));
        }

        private void trackBar1_MouseUp(object sender, MouseEventArgs e)
        {
            SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
        }

        private void trackBar2_MouseUp(object sender, MouseEventArgs e)
        {
            SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            trackBar1.Value = 0;
            trackBar2.Value = 0;
            label15.Text = "0";
            label16.Text = "0";
            radioButton1.Location = new System.Drawing.Point(220, 220);
            SetSpeed(0, 0);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            trackBar1.Value = 0;
            trackBar2.Value = 0;
            label15.Text = "0";
            label16.Text = "0";
            radioButton1.Location = new System.Drawing.Point(220, 220);
            SetBrake(400, 400);
        }

        private void trackBar1_KeyUp(object sender, KeyEventArgs e)
        {
            SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
        }

        private void trackBar2_KeyUp(object sender, KeyEventArgs e)
        {
            SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
        }

        private void groupBox4_Enter(object sender, EventArgs e)
        {

        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private int Clmp(int val, int min, int max)
        {
            if (val.CompareTo(min) < 0) return min;
            else if (val.CompareTo(max) > 0) return max;
            else return val;
        }

        System.Drawing.Point TranslateToCoords(System.Drawing.Point H)
        {
            return new System.Drawing.Point(H.X - 220, 220 - H.Y);
        }

        System.Drawing.Point TranslateBackCoords(System.Drawing.Point H)
        {
            return new System.Drawing.Point(H.X + 220, 220 - H.Y);
        }

        System.Drawing.Point TranslateToRealSpeeds()
        {
            return new System.Drawing.Point();
        }

        bool dragging = false;
        long LastSendTickRadio = 0;

        void UpdateRadioSpeed()
        {
            System.Drawing.Point A = PointToClient(MousePosition);
            System.Drawing.Point B = new System.Drawing.Point(Clmp((A.X - groupBox4.Location.X), 0, 440), Clmp((A.Y - groupBox4.Location.Y), 0, 440));
            System.Drawing.Point H = TranslateToCoords(B);
            double hoek = Math.Atan2(H.Y, H.X) - (Math.PI / 2.0);
            
            if (Math.Sqrt(H.X * H.X + H.Y * H.Y) <= 220)
            {
                radioButton1.Location = B;
            }
            else
            {
                H.X = (int)(220.0 * Math.Sin(-hoek));
                H.Y = (int)(220.0 * Math.Cos(-hoek));
                B = TranslateBackCoords(H);
                B = new System.Drawing.Point(Clmp((B.X), 0, 440), Clmp((B.Y), 0, 440));
                radioButton1.Location = B;
            }

            double divider = Math.Sqrt(H.X * H.X + H.Y * H.Y) / 220.0;
            hoek = Math.Atan2(H.Y, H.X);
            H.X = (int)(divider * (220.0 * (0.5 * Math.Cos(-1 * (-hoek)) + Math.Sin(-1 * (-hoek)))));
            H.Y = (int)(divider * (220.0 * (-0.5 * Math.Cos(-1 * (-hoek)) + Math.Sin(-1 * (-hoek)))));
            H = new System.Drawing.Point(Clmp((H.X), -220, 220), Clmp((H.Y), -220, 220));

            trackBar1.Value = H.X;
            trackBar2.Value = H.Y;
            label15.Text = H.X.ToString();
            label16.Text = H.Y.ToString();
            this.Refresh();
        }

        void UpdateRadioSpeedPos(System.Drawing.Point offset)
        {
            System.Drawing.Point B = new System.Drawing.Point(Clmp((radioButton1.Location.X + offset.X), 0, 440), Clmp((radioButton1.Location.Y + offset.Y), 0, 440));
            System.Drawing.Point H = TranslateToCoords(B);
            double hoek = Math.Atan2(H.Y, H.X) - (Math.PI / 2.0);

            if (Math.Sqrt(H.X * H.X + H.Y * H.Y) <= 220)
            {
                radioButton1.Location = B;
            }
            else
            {
                H.X = (int)(220.0 * Math.Sin(-hoek));
                H.Y = (int)(220.0 * Math.Cos(-hoek));
                B = TranslateBackCoords(H);
                B = new System.Drawing.Point(Clmp((B.X), 0, 440), Clmp((B.Y), 0, 440));
                radioButton1.Location = B;
            }

            double divider = Math.Sqrt(H.X * H.X + H.Y * H.Y) / 220.0;
            hoek = Math.Atan2(H.Y, H.X);
            H.X = (int)(divider * (220.0 * (0.5 * Math.Cos(-1 * (-hoek)) + Math.Sin(-1 * (-hoek)))));
            H.Y = (int)(divider * (220.0 * (-0.5 * Math.Cos(-1 * (-hoek)) + Math.Sin(-1 * (-hoek)))));
            H = new System.Drawing.Point(Clmp((H.X), -220, 220), Clmp((H.Y), -220, 220));

            trackBar1.Value = H.X;
            trackBar2.Value = H.Y;
            label15.Text = H.X.ToString();
            label16.Text = H.Y.ToString();
            this.Refresh();
        }

        private void radioButton1_MouseDown(object sender, MouseEventArgs e)
        {
            dragging = true;
            UpdateRadioSpeed();
        }

        private void radioButton1_MouseUp(object sender, MouseEventArgs e)
        {
            dragging = false;
            UpdateRadioSpeed();

            SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
            LastSendTickRadio = GetTickCount64();
        }

        private void radioButton1_MouseMove(object sender, MouseEventArgs e)
        {
            if (dragging)
            {
                UpdateRadioSpeed();

                if (GetTickCount64() - LastSendTickRadio > 50)
                {
                    SetSpeed((Int16)trackBar1.Value, (Int16)trackBar2.Value);
                    LastSendTickRadio = GetTickCount64();
                }    
            }
        }

        bool Mover = false;
        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.ShiftKey)
            {
                System.Drawing.Point pt = new System.Drawing.Point(this.Location.X + groupBox4.Location.X + (groupBox4.Size.Width / 2), this.Location.Y + groupBox4.Location.Y + (groupBox4.Size.Height / 2));
                Cursor.Position = pt;
                Mover = true;
            }
            if(e.KeyCode == Keys.OemQuestion)
            {
                trackBar1.Value = 0;
                trackBar2.Value = 0;
                label15.Text = "0";
                label16.Text = "0";
                radioButton1.Location = new System.Drawing.Point(220, 220);
                SetBrake(400, 400);
            }
        }

        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.ShiftKey)
            {
                Mover = false;
            }
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {

        }

        private void MouseUpdater_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (Mover)
            {
                System.Drawing.Point pt = new System.Drawing.Point(this.Location.X + groupBox4.Location.X + (groupBox4.Size.Width / 2), this.Location.Y + groupBox4.Location.Y + (groupBox4.Size.Height / 2));
                System.Drawing.Point moved = new System.Drawing.Point((int)((MousePosition.X - pt.X) / 1.5), (int)((MousePosition.Y - pt.Y) / 1.5));
                UpdateRadioSpeedPos(moved);
                Cursor.Position = pt;   
            }
        }
        private void groupBox4_MouseCaptureChanged(object sender, EventArgs e)
        {

        }

        private void verticalProgressBar4_Load(object sender, EventArgs e)
        {

        }

        bool videostarted = false;

        private FilterInfoCollection VideoCaptureDevices;
        private VideoCaptureDevice FinalVideo;

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            if (videostarted)
                return;
            videostarted = true;

            pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
            
            VideoCaptureDevices = new FilterInfoCollection(FilterCategory.VideoInputDevice);

            FinalVideo = new VideoCaptureDevice(VideoCaptureDevices[4].MonikerString);

            FinalVideo.NewFrame += new NewFrameEventHandler(FinalVideo_NewFrame);
            FinalVideo.Start();
        }

        //int counter = 0;
        void FinalVideo_NewFrame(object sender, NewFrameEventArgs eventArgs)
        {
            //++counter;
            //if (counter == 2)
            //{
                Bitmap video = (Bitmap)eventArgs.Frame.Clone();
                pictureBox1.Image = video;
                System.GC.Collect();
                System.GC.WaitForPendingFinalizers();
                //counter = 0;
            //}
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (videostarted)
            {
                FinalVideo.Stop();
            }
        }
    }
}
