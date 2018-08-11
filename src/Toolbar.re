[@bs.val] external document : 'jsModule = "document";

type state = {directoriesEnabled: bool};

type action =
  | SetDirectoriesEnabled(bool)
  | Keydown(ReactEventRe.Keyboard.t);

let component = ReasonReact.reducerComponent("Toolbar");
let make =
    (
      ~dirs,
      ~imageCount,
      ~mode,
      ~move,
      ~pwd,
      ~root,
      ~searchActive,
      ~setImages,
      ~setMode,
      ~setPwd,
      ~setSearchActive,
      ~zoom: bool => unit,
      _children,
    ) => {
  ...component,
  initialState: () => {directoriesEnabled: true},
  didMount: self =>
    document##addEventListener("keydown", e => self.send(Keydown(e))),
  reducer: (action, state) =>
    switch (action) {
    | SetDirectoriesEnabled(b) => ReasonReact.Update({directoriesEnabled: b})
    | Keydown(e) =>
      let active = document##activeElement;
      let notInput = active##tagName != "INPUT";
      let isInputFilter = active##className == "filter";
      let noModifiers =
        ! (
          ReactEventRe.Keyboard.altKey(e)
          || ReactEventRe.Keyboard.ctrlKey(e)
          || ReactEventRe.Keyboard.metaKey(e)
          || ReactEventRe.Keyboard.shiftKey(e)
        );
      switch (ReactEventRe.Keyboard.key(e)) {
      | "n" when notInput && noModifiers =>
        ReactEventRe.Keyboard.preventDefault(e);
        ReasonReact.Update({directoriesEnabled: true});
      | "Escape" when (notInput || isInputFilter) && state.directoriesEnabled =>
        ReasonReact.Update({directoriesEnabled: false})
      | _ => ReasonReact.NoUpdate
      };
    },
  render: self => {
    let search = (query: string) => {
      setSearchActive(true);
      setImages(Search.search(root, query));
    };

    let cancel = () => {
      setSearchActive(false);
      setImages(Path.images(pwd));
    };

    let menuItems: Js.Array.t(PopupMenu.item) = [|
      {text: "Move", action: () => ()},
      {text: "Nightmode", action: () => {
        Util.toggleNightMode();
      }},
    |];

    let renderPath = (p: Path.base) : string =>
      Path.renderable(p, pwd, root);

    let onMouseLeave = _e => {
      let _ =
        Js.Global.setTimeout(
          _ => self.ReasonReact.send(SetDirectoriesEnabled(false)),
          200,
        );
      ();
    };

    let onMouseEnter = _e =>
      self.ReasonReact.send(SetDirectoriesEnabled(true));

    /* Construct PWD */
    let renderedPwd = {
      let pwdPath = Path.renderable(pwd, pwd, root);
      {j|$(pwdPath) ($(imageCount))|j};
    };

    <header className="main-header" onMouseLeave>
      (
        if (! searchActive) {
          <Directories
            title="Navigate:"
            items=(Array.of_list(dirs))
            setPwd
            enabled=self.state.directoriesEnabled
            setEnabled=(b => self.send(SetDirectoriesEnabled(b)))
            renderPath
          />;
        } else {
          ReasonReact.null;
        }
      )
      /* Define separate zones for separate on-hover behavior */
      <div className="toolbar">
        <div className="left" onMouseEnter>
          (
            if (! searchActive) {
              <h3> (ReasonReact.string(renderedPwd)) </h3>;
            } else {
              ReasonReact.null;
            }
          )
        </div>
        <div className="center">
          <a href="#" onClick=(_ => zoom(false))>
            (ReasonReact.string("-"))
          </a>
          (ReasonReact.string("Zoom"))
          <a href="#" onClick=(_ => zoom(true))>
            (ReasonReact.string("+"))
          </a>
        </div>
        <div className="right">
          <Search active=searchActive search cancel />
          <PopupMenu title="More" menuItems />
        </div>
      </div>
    </header>;
    /* <Edit mode pwd root move onClick=((m, _) => setMode(m)) /> */
  },
};
