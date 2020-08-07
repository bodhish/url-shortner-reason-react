[@react.component]
let make = () => {
  let (code, setCode) = React.useState(_ => None);
  <div className="w-full flex md:flex-row flex-col ">
    {switch (code) {
     | Some(code) =>
       <div className="w-full text-center">
         <div className="inline-block tracking-wide text-xs font-semibold">
           {"Your short code is" |> React.string}
         </div>
         <div
           className="text-center ml-2 py-3 px-4 md:text-md leading-4 font-medium rounded w-full font-mono bg-gray-200 rounded-lg break-words">
           {Config.frontendHost ++ "r/" ++ code |> React.string}
         </div>
       </div>
     | None =>
       <ShortCodeGenerator__Form callbackCB={code => setCode(_ => code)} />
     }}
  </div>;
};
