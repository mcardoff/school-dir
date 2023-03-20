(TeX-add-style-hook
 "Lab_4_Lecture"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("physics" "italicdiff")))
   (TeX-run-style-hooks
    "latex2e"
    "beamer"
    "beamer10"
    "physics"
    "amsmath"
    "amsthm"
    "hyperref"
    "url"
    "multicol")
   (TeX-add-symbols
    "dbar")
   (LaTeX-add-environments
    '("itemframe" 1))
   (LaTeX-add-amsthm-newtheorems
    "law"))
 :latex)

