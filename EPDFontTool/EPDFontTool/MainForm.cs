using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace EPDFontTool {
    public partial class MainForm : Form {

        const string LINE_END = "\r\n";
        const string INDENT = "    ";

        private Image image = null;

        public MainForm() {
            InitializeComponent();
        }

        private void openImageButton_Click(object sender, EventArgs e) {
            if (openFileDialog.ShowDialog() == DialogResult.Cancel) return;

            image = Image.FromFile(openFileDialog.FileName);

            selectedImageLabel.Text = "Selected Image: " + Path.GetFileName(openFileDialog.FileName);
            imageBox.Image = image;
            dataSizeBox.Text = (image.Width * image.Height / 8).ToString();
        }

        private byte GetImageByte(Bitmap bm, int x, int firstY) {
            byte result = 0;
            for (int i = 0; i < 8; i++) {
                int y = firstY + i;
                result <<= 1;

                var color = bm.GetPixel(x, y);
                var pixel = (byte)((color.GetBrightness() < 0.5) ? 1 : 0);
                result |= pixel;
            }
            if (invertBox.Checked) return (byte)~result;
            else return result;
        }

        private void generateButton_Click(object sender, EventArgs e) {
            var charHeightBytes = image.Height / 8;

            if (charHeightBytes % 2 != 0) {
                MessageBox.Show("Image height needs to be a multiple of 16 pixels");
                return;
            }

            var numChars = imageCharBox.Text.Length;

            if (numChars == 0) {
                MessageBox.Show("Must specify at least one character.");
                return;
            }
            if (image.Width % numChars != 0) {
                MessageBox.Show("Image width needs to be a multiple of the number of characters");
                return;
            }

            var name = nameBox.Text;

            if (nameBox.Text.Length == 0) {
                MessageBox.Show("Name cannot be empty.");
                return;
            }

            if (saveFileDialog.ShowDialog() == DialogResult.Cancel) return;

            var charWidth = image.Width / numChars;
            
            var totalBytes = image.Width * charHeightBytes;

            var bm = new Bitmap(image);

            var nameUpper = name.ToUpperInvariant();

            var enc = Encoding.GetEncoding(1252);
            var imageChars = enc.GetBytes(imageCharBox.Text);
            var charOffsets = new Dictionary<byte, int>();
            for (int i = 0; i < imageChars.Length; i++) {
                charOffsets[imageChars[i]] = (imageChars.Length - i - 1) * charHeightBytes * charWidth;
            }

            var sb = new StringBuilder();
            //header comment
            sb.AppendFormat("/*{0}EPDFontTool generated header{0}Font name: {1}{0}*/{0}", LINE_END, name);
            //multiple inclusion guard top
            sb.AppendFormat("#ifndef {1}_H{0}#define {1}_H{0}{0}", LINE_END, nameUpper);
            //include
            sb.AppendFormat("#include <stdint.h>{0}{0}", LINE_END);
            //defines
            sb.AppendFormat("#define FONT_{1}_CHAR_WIDTH {2}{0}", LINE_END, name, charWidth);
            sb.AppendFormat("#define FONT_{1}_CHAR_HEIGHT {2}{0}", LINE_END, name, image.Height);
            sb.AppendFormat("#define FONT_{1}_CHAR_HEIGHT_BYTES {2}{0}", LINE_END, name, charHeightBytes);
            //cpp guard top
            sb.AppendFormat("{0}#ifdef __cplusplus{0}extern \"C\" {{{0}#endif /* __cplusplus */{0}{0}", LINE_END);
            //mapping
            var maxOffset = charOffsets.Values.Max();
            var maxLen = (int)Math.Max(Math.Log10(maxOffset) + 1, 2);
            var offsetFormat = string.Format("{{0,{0}}}, ", maxLen);
            sb.AppendFormat("{0}const int32_t {1}_char_offsets[256] = {{ ", INDENT, name);
            for (int i = 0; i < 16; i++) {
                sb.Remove(sb.Length - 1, 1).Append(LINE_END).Append(INDENT).Append(INDENT);
                for (int j = 0; j < 16; j++) {
                    byte ch = (byte)(16 * i + j);
                    var offset = charOffsets.ContainsKey(ch) ? charOffsets[ch] : -1;
                    sb.AppendFormat(offsetFormat, offset);
                }
            }
            sb.Remove(sb.Length - 2, 2).AppendFormat("{0}{1}}};{0}{0}", LINE_END, INDENT);
            //main data
            var bytesPerLine = 20;
            while (totalBytes % bytesPerLine != 0) bytesPerLine--;
            sb.AppendFormat("{0}const uint8_t __attribute__((space(prog))) {1}_data[{2}] = {{ ", INDENT, name, totalBytes);
            var bytesInCurrentLine = bytesPerLine;
            for (int i = image.Width - 1; i >= 0; i--) {
                for (int j = 0; j < image.Height; j += 8) {
                    if (bytesInCurrentLine++ >= bytesPerLine) {
                        sb.Remove(sb.Length - 1, 1).Append(LINE_END).Append(INDENT).Append(INDENT);
                        bytesInCurrentLine = 1;
                    }
                    sb.AppendFormat("0x{0:X2}, ", GetImageByte(bm, i, j));
                }
            }
            sb.Remove(sb.Length - 2, 2).AppendFormat("{0}{1}}};{0}{0}", LINE_END, INDENT);
            //cpp guard bottom
            sb.AppendFormat("#ifdef __cplusplus{0}}}{0}#endif /* __cplusplus */{0}{0}", LINE_END);
            //multiple inclusion guard bottom
            sb.AppendFormat("#endif /* {1}_H */{0}", LINE_END, nameUpper);

            File.WriteAllText(saveFileDialog.FileName, sb.ToString());
        }
    }
}
