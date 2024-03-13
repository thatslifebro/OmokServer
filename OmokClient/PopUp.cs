using csharp_test_client;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace OmokClient
{
    public partial class PopUp : Form
    {
        public PopUp()
        {
            InitializeComponent();
        }

        public delegate void DataPassEventHandler(bool accept);

        public event DataPassEventHandler DataPassEvent;

        public void ChangeLabel(string adminId)
        {
            label.Text = adminId + "님이 오목을 신청했습니다.\n수락하시겠습니까?";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            DataPassEvent(true);
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DataPassEvent(false);
            this.Close();
        }
    }
}
