
namespace EPDFontTool {
    partial class MainForm {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.openImageButton = new System.Windows.Forms.Button();
            this.selectedImageLabel = new System.Windows.Forms.Label();
            this.invertBox = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.imageCharBox = new System.Windows.Forms.TextBox();
            this.imageBox = new System.Windows.Forms.PictureBox();
            this.generateButton = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.nameBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.dataSizeBox = new System.Windows.Forms.TextBox();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            ((System.ComponentModel.ISupportInitialize)(this.imageBox)).BeginInit();
            this.SuspendLayout();
            // 
            // openImageButton
            // 
            this.openImageButton.Location = new System.Drawing.Point(12, 12);
            this.openImageButton.Name = "openImageButton";
            this.openImageButton.Size = new System.Drawing.Size(86, 23);
            this.openImageButton.TabIndex = 0;
            this.openImageButton.Text = "Open Image...";
            this.openImageButton.UseVisualStyleBackColor = true;
            this.openImageButton.Click += new System.EventHandler(this.openImageButton_Click);
            // 
            // selectedImageLabel
            // 
            this.selectedImageLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.selectedImageLabel.Location = new System.Drawing.Point(104, 12);
            this.selectedImageLabel.Name = "selectedImageLabel";
            this.selectedImageLabel.Size = new System.Drawing.Size(286, 23);
            this.selectedImageLabel.TabIndex = 1;
            this.selectedImageLabel.Text = "Selected Image:";
            this.selectedImageLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // invertBox
            // 
            this.invertBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.invertBox.AutoSize = true;
            this.invertBox.Location = new System.Drawing.Point(12, 129);
            this.invertBox.Name = "invertBox";
            this.invertBox.Size = new System.Drawing.Size(212, 17);
            this.invertBox.TabIndex = 2;
            this.invertBox.Text = "Invert (white becomes black on display)";
            this.invertBox.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 155);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(136, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Characters (in image order):";
            // 
            // imageCharBox
            // 
            this.imageCharBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.imageCharBox.Location = new System.Drawing.Point(154, 152);
            this.imageCharBox.Name = "imageCharBox";
            this.imageCharBox.Size = new System.Drawing.Size(236, 20);
            this.imageCharBox.TabIndex = 4;
            // 
            // imageBox
            // 
            this.imageBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.imageBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.imageBox.Location = new System.Drawing.Point(12, 41);
            this.imageBox.Name = "imageBox";
            this.imageBox.Size = new System.Drawing.Size(378, 82);
            this.imageBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.imageBox.TabIndex = 5;
            this.imageBox.TabStop = false;
            // 
            // generateButton
            // 
            this.generateButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.generateButton.Location = new System.Drawing.Point(12, 230);
            this.generateButton.Name = "generateButton";
            this.generateButton.Size = new System.Drawing.Size(378, 23);
            this.generateButton.TabIndex = 6;
            this.generateButton.Text = "Generate header...";
            this.generateButton.UseVisualStyleBackColor = true;
            this.generateButton.Click += new System.EventHandler(this.generateButton_Click);
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 207);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "Name:";
            // 
            // nameBox
            // 
            this.nameBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.nameBox.Location = new System.Drawing.Point(56, 204);
            this.nameBox.Name = "nameBox";
            this.nameBox.Size = new System.Drawing.Size(334, 20);
            this.nameBox.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 181);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(110, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Font data size (bytes):";
            // 
            // dataSizeBox
            // 
            this.dataSizeBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dataSizeBox.Location = new System.Drawing.Point(128, 178);
            this.dataSizeBox.Name = "dataSizeBox";
            this.dataSizeBox.ReadOnly = true;
            this.dataSizeBox.Size = new System.Drawing.Size(262, 20);
            this.dataSizeBox.TabIndex = 10;
            // 
            // openFileDialog
            // 
            this.openFileDialog.Filter = "PNG files|*.png|All files|*.*";
            // 
            // saveFileDialog
            // 
            this.saveFileDialog.Filter = "Header files|*.h";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(402, 265);
            this.Controls.Add(this.dataSizeBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.nameBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.generateButton);
            this.Controls.Add(this.imageBox);
            this.Controls.Add(this.imageCharBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.invertBox);
            this.Controls.Add(this.selectedImageLabel);
            this.Controls.Add(this.openImageButton);
            this.MinimumSize = new System.Drawing.Size(418, 304);
            this.Name = "MainForm";
            this.Text = "EPD Font Tool";
            ((System.ComponentModel.ISupportInitialize)(this.imageBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button openImageButton;
        private System.Windows.Forms.Label selectedImageLabel;
        private System.Windows.Forms.CheckBox invertBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox imageCharBox;
        private System.Windows.Forms.PictureBox imageBox;
        private System.Windows.Forms.Button generateButton;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox nameBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox dataSizeBox;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.SaveFileDialog saveFileDialog;
    }
}

