# ShaderEmu Mini Fighter

## 專案用途

這是一個以小畫家風格（LF2-style）呈現的極簡 2D 格鬥遊戲，使用純 C 語言撰寫，圖形部分採用 Nano-X API，適合在資源有限的環境中編譯與執行。

## ShaderEmu / VRChat 使用情境

本專案鎖定 VRChat 中的 **ShaderEmu RISC-V Linux 模擬電腦**。該環境內建 **TinyCC**（C 編譯器）、**Nano-X**（圖形函式庫）與 IDE，可直接編譯這份單一檔案原始碼。由於 VRChat 的文字輸入框會移除換行，建議透過 Raw HTTPS 網址下載原始碼，而不是直接貼上完整程式。

## 原始碼

- 編譯目標檔案：`fighter.c`
- 原始下載檔名：`lf2_mini_shaderemu.c`
- 內容與原始檔案完全一致，未修改程式邏輯。

## 操作按鍵

| 按鍵 | 功能 |
| --- | --- |
| `A` / `D` | 向左 / 向右移動 |
| `W` / `S` | 向前 / 向後移動（切換縱深） |
| `SPACE` | 跳躍 |
| `J` | 出拳 |
| `K` | 踢擊 |
| `L` | 發射氣功 |
| `R` | 重新開始 |
| `ESC` | 離開遊戲 |

## 驗證狀態

此原始碼**尚未經 ShaderEmu 實機編譯與執行驗證**。Windows 端僅完成原始碼內容、行數、大小與 SHA256 的一致性檢查；實際能否在 VRChat 的 ShaderEmu 中成功編譯與遊玩，仍需由使用者在該環境中測試。
