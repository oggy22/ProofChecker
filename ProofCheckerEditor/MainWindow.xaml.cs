using Microsoft.Win32;
using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ProofCheckerEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        string filename; // = Directory.GetCurrentDirectory() + @"\..\..\..\Theories\groupoid.thr";
        bool saved;
        public static RoutedUICommand checkCommand = new RoutedUICommand("checkCommand", "checkCommand", typeof(MainWindow));

        public MainWindow()
        {
            InitializeComponent();
            loadFileName();
            LoadFile(filename);
        }

        private void loadFileName()
        {
            string file = string.Empty;
            try { file = System.IO.File.ReadAllText("filename.txt"); }
            catch (Exception)
            { }
            if (file != string.Empty)
                filename = file;
        }

        private void LoadFile(string filename)
        {
            try
            {
                this.txtSource.Text = System.IO.File.ReadAllText(filename);
            }
            catch (Exception)
            {
                this.txtSource.Text = "// Type your theory here";
                filename = "Untitled1.thr";
            }
            saved = true;
            updateTitle();
            txtSource.Focus();
            System.IO.File.WriteAllText("filename.txt", filename);
        }

        #region Events
        private void InsertButtonClicked(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            string str = button.Content.ToString();

            string stringToAdd = (sender as Button).Content.ToString();
            txtSource.SelectedText = stringToAdd;
            txtSource.Select(txtSource.SelectionStart+stringToAdd.Length, 0);
            txtSource.Focus();
        }

        private void New_Clicked(object sender, RoutedEventArgs e)
        {
            filename = null;
            saved = false;
            txtSource.Text = string.Empty;
            updateTitle();
        }

        private void Open_Clicked(object sender, RoutedEventArgs e)
        {
            // Create an instance of the open file dialog box.
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            // Set filter options and filter index.
            openFileDialog1.Filter = "Theory File (.thr)|*.thr|All Files (*.*)|*.*";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.Multiselect = false;
            openFileDialog1.InitialDirectory = @"D:\C++\ProofChecker\Theories";

            // Call the ShowDialog method to show the dialog box.
            bool? userClickedOK = openFileDialog1.ShowDialog();

            // Process input if the user clicked OK.
            if (userClickedOK == true)
            {
                filename = openFileDialog1.FileName;
                LoadFile(filename);
            }
        }

        private void Save_Clicked(object sender, RoutedEventArgs e)
        {
            System.IO.File.WriteAllText(filename, txtSource.Text);
            saved = true;
            updateTitle();
        }

        private void SaveAs_Clicked(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();

            // Set filter options and filter index.
            saveFileDialog.Filter = "Theory File (.thr)|*.thr|All Files (*.*)|*.*"; 
            saveFileDialog.FilterIndex = 1;
            saveFileDialog.InitialDirectory = @"D:\C++\ProofChecker\Theories";

            if (saveFileDialog.ShowDialog() == true)
            {
                filename = saveFileDialog.FileName;
                Save_Clicked(null, null);
            }
        }

        private void About_Click(object sender, RoutedEventArgs e)
        {
            string version = System.Reflection.Assembly.GetExecutingAssembly()
                                                       .GetName()
                                                       .Version
                                                       .ToString();
            MessageBox.Show("ProofChecker Editor verision = " + version);
        }

        private void Check_Click(object sender, RoutedEventArgs e)
        {
            // Save if not saved already
            if (!saved)
            {
                Save_Clicked(this, null);
            }

            txtCompiler.Text = string.Empty;
            RunWithRedirect("ProofChecker.exe");
        }

        void RunWithRedirect(string cmdPath)
        {
            var proc = new Process();
            proc.StartInfo.FileName = cmdPath;
            proc.StartInfo.Arguments = filename;

            // set up output redirection
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardError = true;
            proc.EnableRaisingEvents = true;
            proc.StartInfo.CreateNoWindow = true;
            proc.StartInfo.UseShellExecute = false;

            // see below for output handler
            proc.ErrorDataReceived += proc_DataReceived;
            proc.OutputDataReceived += proc_DataReceived;

            proc.Start();

            proc.BeginErrorReadLine();
            proc.BeginOutputReadLine();
        }

        void proc_DataReceived(object sender, DataReceivedEventArgs e)
        {
            txtCompiler.Dispatcher.Invoke(
                () => { txtCompiler.Text += e.Data + "\n"; });
        }

        private void txtSource_TextChanged(object sender, TextChangedEventArgs e)
        {
            saved = false;
            updateTitle();

        }
        #endregion

        private void updateTitle()
        {
            this.Title = "Proof Checker Editor - "
                + (filename == null ? "newfile" : System.IO.Path.GetFileName(filename))
                + (saved ? "" : "*");
        }

        private void Exit_Clicked(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void txtCompiler_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            string error = txtCompiler.Text.Split(new []{'\n', '\r'}, StringSplitOptions.RemoveEmptyEntries).Last();
            if (error[0] != '(')
                return;

            string[] parts = error.Split(new[] { '(', ',', ')' }, StringSplitOptions.RemoveEmptyEntries);
            int line = int.Parse(parts[0]);
            int column = int.Parse(parts[1]);
            this.txtSource.Focus();
            

            for (int pos=0; pos<txtSource.Text.Length; pos++)
            {
                if (line==1)
                {
                    if (column==1)
                        txtSource.Select(pos, 0);
                    column--;
                }
                if (txtSource.Text[pos] == '\n')
                    line--;
            }
        }

        private void CanExecuteSaveHandler(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = !saved;            
        }
    }
}