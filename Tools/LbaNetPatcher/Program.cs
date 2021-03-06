﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace LbaNetPatcher
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            MainWindow win = new MainWindow();
            Application.ApplicationExit += new EventHandler(win.OnApplicationExit);
            Application.Run(win);
        }
    }
}
