[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  <div className="h-screen flex flex-col justify-center items-center px-4">
    <h1 className="font-bold text-3xl text-center">
      {"URL Shortner" |> React.string}
    </h1>
    <div
      className="max-w-lg rounded overflow-hidden shadow-lg p-4 mt-4 w-full">
      {switch (url.path) {
       | ["r", code] => <FindUrl code />
       | _ => <ShortCodeGenerator />
       }}
    </div>
  </div>;
};
