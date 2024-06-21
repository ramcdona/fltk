
// Needed for main()
#include <FL/Fl_Double_Window.H>


// Needed for everything else
#include "SpreadSheetWidget.h"
#include <cfloat>
#include <math.h>

// strtok replacement from https://stackoverflow.com/questions/26522583/c-strtok-skips-second-token-or-consecutive-delimiter
// ... modified to fix 'const' and 'static' attributes, remove superfluous variable 'tkn'
char *paxtok(char *str, const char *seps) {
  static char *pos = NULL;
  char *tpos;
  static char savech;

  // Specific actions for first and subsequent calls.

  if (str != NULL) {
    // First call, set pointer.

    pos = str;
    savech = 'x';
  } else {
    // Subsequent calls, check we've done first.

    if (pos == NULL)
      return NULL;

    // Then put character back and advance.

    while (*pos != '\0')
      pos++;
    *pos++ = savech;
  }

  // Detect previous end of string.

  if (savech == '\0')
    return NULL;

  // Now we have pos pointing to first character.
  // Find first separator or nul.

  tpos = pos;
  while (*tpos != '\0') {
    if (strchr(seps, *tpos))
      break;
    tpos++;
  }

  savech = *tpos;
  *tpos = '\0';

  return pos;
}

void parse_table(const char *str, int len, std::vector<std::vector<string> > &table) {
  // strtok won't work on const char *, so make a copy.
  char str_copy[len + 1];
  strcpy(str_copy, str);
  str_copy[len] = '\0';

  // Parse lines by \n
  std::vector<string> lines;
  char *l = paxtok(str_copy, "\n");
  while (l != nullptr) {
    lines.emplace_back(l);
    l = paxtok(nullptr, "\n");
  }

  table.resize(lines.size());
  int ncol = 0;
  for (int i = 0; i < (int)lines.size(); i++) {
    // strtok won't work on const char *, so make a copy.
    int siz = lines[i].size();
    char buf[siz + 1];
    strcpy(buf, lines[i].c_str());
    buf[siz] = '\0';

    // Parse fields by \t
    char *f = paxtok(buf, "\t");
    while (f != nullptr) {
      table[i].emplace_back(f);
      f = paxtok(nullptr, "\t");
    }

    if ((int)table[i].size() > ncol) {
      ncol = table[i].size();
    }
  }

  // ensure all rows have same number of columns.
  for (int i = 0; i < (int)table.size(); i++) {
    table[i].resize(ncol);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

template <> void SpreadSheet<int>::set_value(int R, int C, const char *str) {
  (*m_Data)[R] = strtol(str, nullptr, 10);
}

template <> string SpreadSheet<int>::get_value(int R, int C) {
  char s[255];
  snprintf(s, sizeof(s), "%d", (*m_Data)[R]);
  return string(s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <> void SpreadSheet<double>::set_value(int R, int C, const char *str) {
  (*m_Data)[R] = strtod(str, nullptr);
}

template <> string SpreadSheet<double>::get_value(int R, int C) {
  char s[255];
  snprintf(s, sizeof(s), "%f", (*m_Data)[R]);
  return string(s);
}

template <> string SpreadSheet<double>::get_exact_value(int R, int C) {
  char s[255];
  snprintf(s, sizeof(s), "%.*e", DBL_DIG + 3, (*m_Data)[R]);
  return string(s);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <> void SpreadSheet<string>::set_value(int R, int C, const char *str) {
  (*m_Data)[R] = string(str);
}

template <> string SpreadSheet<string>::get_value(int R, int C) {
  return (*m_Data)[R];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <> void SpreadSheet<vector<int> >::set_value(int R, int C, const char *str) {
  (*m_Data)[R][C] = strtol(str, nullptr, 10);
}

template <> string SpreadSheet<vector<int> >::get_value(int R, int C) {
  char s[255];
  snprintf(s, sizeof(s), "%d", (*m_Data)[R][C]);
  return string(s);
}

// Unfortunately, the following two methods must be re-specialized for every
// vector type.  Fortunately, copy/paste should suffice as they are data type
// independent.
template <> int SpreadSheet<vector<int> >::get_data_ncol() {
  if (get_data_nrow() > 0) {
    return (*m_Data)[0].size();
  }
  return 0;
};

template <> void SpreadSheet<vector<int> >::update_size(int nrow, int ncol) {
  bool change = false;

  if (get_data_nrow() != nrow) {
    (*m_Data).resize(nrow);
    change = true;
  }

  if ((get_data_ncol() != ncol) || change) {
    for (int i = 0; i < get_data_nrow(); i++) {
      (*m_Data)[i].resize(ncol);
    }
    change = true;
  }

  if (change) {
    update_size();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <> void SpreadSheet<vector<double> >::set_value(int R, int C, const char *str) {
  (*m_Data)[R][C] = strtod(str, nullptr);
}

template <> string SpreadSheet<vector<double> >::get_value(int R, int C) {
  char s[255];
  snprintf(s, sizeof(s), "%f", (*m_Data)[R][C]);
  return string(s);
}

template <> string SpreadSheet<vector<double> >::get_exact_value(int R, int C) {
  char s[255];
  snprintf(s, sizeof(s), "%.*e", DBL_DIG + 3, (*m_Data)[R][C]);
  return string(s);
}

// Unfortunately, the following two methods must be re-specialized for every
// vector type.  Fortunately, copy/paste should suffice as they are data type
// independent.
template <> int SpreadSheet<vector<double> >::get_data_ncol() {
  if (get_data_nrow() > 0) {
    return (*m_Data)[0].size();
  }
  return 0;
};

template <> void SpreadSheet<vector<double> >::update_size(int nrow, int ncol) {
  bool change = false;

  if (get_data_nrow() != nrow) {
    (*m_Data).resize(nrow);
    change = true;
  }

  if ((get_data_ncol() != ncol) || change) {
    for (int i = 0; i < get_data_nrow(); i++) {
      (*m_Data)[i].resize(ncol);
    }
    change = true;
  }

  if (change) {
    update_size();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <> void SpreadSheet<vector<string> >::set_value(int R, int C, const char *str) {
  (*m_Data)[R][C] = string(str);
}

template <> string SpreadSheet<vector<string> >::get_value(int R, int C) {
  return (*m_Data)[R][C];
}

// Unfortunately, the following two methods must be re-specialized for every
// vector type.  Fortunately, copy/paste should suffice as they are data type
// independent.
template <> int SpreadSheet<vector<string> >::get_data_ncol() {
  if (get_data_nrow() > 0) {
    return (*m_Data)[0].size();
  }
  return 0;
};

template <> void SpreadSheet<vector<string> >::update_size(int nrow, int ncol) {
  bool change = false;

  if (get_data_nrow() != nrow) {
    (*m_Data).resize(nrow);
    change = true;
  }

  if ((get_data_ncol() != ncol) || change) {
    for (int i = 0; i < get_data_nrow(); i++) {
      (*m_Data)[i].resize(ncol);
    }
    change = true;
  }

  if (change) {
    update_size();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  Fl_Double_Window *win = new Fl_Double_Window(862, 322, "Table Spreadsheet");
  SpreadSheetDouble *table = new SpreadSheet<vector<double> >(10, 10, win->w() - 20, win->h() - 20);
  table->tab_cell_nav(1); // enable tab navigation of table cells (instead of fltk widgets)
  table->tooltip("Use keyboard to navigate cells:\n"
                 "Arrow keys or Tab/Shift-Tab");
  // Table rows
  table->row_header(1);
  table->row_header_width(70);
  table->row_resize(1);
  table->row_default_height(25);
  // Table cols
  table->col_header(1);
  table->col_header_height(25);
  table->col_resize(1);
  table->col_default_width(70);

  int initrow = 5;
  int initcol = 4;
  vector<vector<double> > dblmat;
  dblmat.resize(initrow);
  for (int r = 0; r < initrow; r++) {
    dblmat[r].resize(initcol);
    for (int c = 0; c < initcol; c++) {
      dblmat[r][c] = sin(c + (r * initcol));
    }
  }

  table->set_data(&dblmat);

  // Show window
  win->end();
  win->resizable(table);
  win->show();
  return Fl::run();
}
