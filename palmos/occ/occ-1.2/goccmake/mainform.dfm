object Form1: TForm1
  Left = 415
  Top = 114
  BorderStyle = bsDialog
  Caption = 'gOCCMake v1.0 - www.schau.com/palm/occ'
  ClientHeight = 281
  ClientWidth = 431
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001001010100000000000280100001600000028000000100000002000
    00000100040000000000C0000000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    00000000000000000BBBBB0000000000000000000000000BBBBBBBBB00000000
    0000000000000BBBBBBBBBBBBB0000000000000000000BBB00BB000BB00000B0
    0B00B0B00B0000B00B0000B0000000B00B0000B000000BB00B00B0B00B0000B0
    00BB000BB000000000000000000000000000000000000000000000000000FC1F
    0000F83F0000F0070000E00F0000C001000080030000C4E700008C630000C94B
    0000C9CF0000C9CF000088430000DCE70000FFFF0000FFFF0000FFFF0000}
  OldCreateOrder = False
  Position = poDefault
  OnCreate = resetIconsButtonClick
  PixelsPerInch = 96
  TextHeight = 13
  object urlLabel: TLabel
    Left = 8
    Top = 24
    Width = 22
    Height = 13
    Caption = 'URL'
  end
  object launcherNameLabel: TLabel
    Left = 8
    Top = 56
    Width = 76
    Height = 13
    Caption = 'Launcher Name'
  end
  object urlEdit: TEdit
    Left = 104
    Top = 20
    Width = 313
    Height = 21
    TabOrder = 0
  end
  object launcherNameEdit: TEdit
    Left = 104
    Top = 52
    Width = 313
    Height = 21
    MaxLength = 31
    TabOrder = 1
  end
  object standaloneExecutableCheck: TCheckBox
    Left = 248
    Top = 88
    Width = 169
    Height = 17
    Caption = 'Create Standalone Executable'
    Checked = True
    State = cbChecked
    TabOrder = 2
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 88
    Width = 177
    Height = 169
    Caption = '   Icons   '
    TabOrder = 3
    object largeIconImage: TImage
      Left = 112
      Top = 34
      Width = 32
      Height = 22
      Center = True
    end
    object smallIconImage: TImage
      Left = 112
      Top = 88
      Width = 15
      Height = 9
      Center = True
    end
    object largeIconButton: TButton
      Left = 16
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Large'
      TabOrder = 0
      OnClick = openLargeImage
    end
    object smallIconButton: TButton
      Left = 16
      Top = 80
      Width = 75
      Height = 25
      Caption = 'Small'
      TabOrder = 1
      OnClick = smallIconButtonClick
    end
    object resetIconsButton: TButton
      Left = 16
      Top = 128
      Width = 75
      Height = 25
      Caption = 'Reset Icons'
      TabOrder = 2
      OnClick = resetIconsButtonClick
    end
  end
  object buildButton: TButton
    Left = 344
    Top = 224
    Width = 75
    Height = 25
    Caption = 'Build'
    TabOrder = 4
    OnClick = buildButtonClick
  end
  object statusBar: TStatusBar
    Left = 0
    Top = 262
    Width = 431
    Height = 19
    Panels = <>
    SimplePanel = False
    SizeGrip = False
  end
  object openLargeImageDialog: TOpenDialog
    DefaultExt = '*.bmp'
    Filter = 'BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*'
    Left = 112
    Top = 224
  end
  object openSmallImageDialog: TOpenDialog
    Filter = 'BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*'
    Left = 144
    Top = 224
  end
  object saveOCCDialog: TSaveDialog
    DefaultExt = '.prc'
    Filter = 'PRC Files (*.prc)|*.prc'
    Left = 304
    Top = 224
  end
end
