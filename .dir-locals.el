((c-mode .
	((c-file-style . "linux")
	 (indent-tabs-mode . t)
	 (tab-width . 8)
	 (eval add-hook 'before-save-hook #'clang-format-buffer nil t)
	 )))
