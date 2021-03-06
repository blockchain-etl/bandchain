module Styles = {
  open Css;
  let infoContainer =
    style([
      backgroundColor(Colors.white),
      boxShadow(Shadow.box(~x=`zero, ~y=`px(2), ~blur=`px(4), Css.rgba(0, 0, 0, 0.08))),
      padding(`px(24)),
      position(`relative),
      Media.mobile([padding(`px(16))]),
    ]);
  let idContainer = {
    style([
      selector(
        "> h4",
        [marginLeft(`px(6)), Media.mobile([marginLeft(`zero), marginTop(`px(16))])],
      ),
    ]);
  };
  let badgeContainer = {
    style([Media.mobile([position(`absolute), right(`px(16)), top(`px(16))])]);
  };
};

module ProposalCard = {
  [@react.component]
  let make = (~reserveIndex, ~proposalSub: ApolloHooks.Subscription.variant(ProposalSub.t)) => {
    let isMobile = Media.isMobile();
    <Col.Grid key={reserveIndex |> string_of_int} mb=24 mbSm=16>
      <div className=Styles.infoContainer>
        <Row.Grid marginBottom=18>
          <Col.Grid col=Col.Eight>
            <div
              className={Css.merge([
                CssHelper.flexBox(),
                CssHelper.flexBoxSm(~direction=`column, ~align=`flexStart, ()),
                Styles.idContainer,
              ])}>
              {switch (proposalSub) {
               | Data({id, name}) =>
                 <>
                   <TypeID.Proposal id position=TypeID.Subtitle />
                   <Heading size=Heading.H4 value=name />
                 </>
               | _ =>
                 isMobile
                   ? <>
                       <LoadingCensorBar width=50 height=15 mbSm=16 />
                       <LoadingCensorBar width=150 height=15 mbSm=16 />
                     </>
                   : <LoadingCensorBar width=270 height=15 />
               }}
            </div>
          </Col.Grid>
          <Col.Grid col=Col.Four>
            <div
              className={Css.merge([
                CssHelper.flexBox(~justify=`flexEnd, ()),
                Styles.badgeContainer,
              ])}>
              {switch (proposalSub) {
               | Data({status}) => <ProposalBadge status />
               | _ => <LoadingCensorBar width=100 height=15 radius=50 />
               }}
            </div>
          </Col.Grid>
        </Row.Grid>
        <Row.Grid marginBottom=24>
          <Col.Grid>
            {switch (proposalSub) {
             | Data({description}) => <Markdown value=description />
             | _ => <LoadingCensorBar width=270 height=15 />
             }}
          </Col.Grid>
        </Row.Grid>
        <Row.Grid>
          <Col.Grid col=Col.Four mbSm=16>
            <Heading value="Proposer" size=Heading.H5 marginBottom=8 />
            {switch (proposalSub) {
             | Data({proposerAddress}) =>
               <AddressRender address=proposerAddress position=AddressRender.Subtitle />
             | _ => <LoadingCensorBar width=270 height=15 />
             }}
          </Col.Grid>
          <Col.Grid col=Col.Four colSm=Col.Seven>
            <div className={CssHelper.mb(~size=8, ())}>
              {switch (proposalSub) {
               | Data({status}) =>
                 <Heading
                   value={
                     switch (status) {
                     | Deposit => "Deposit End Time"
                     | Voting
                     | Passed
                     | Rejected
                     | Failed => "Voting End Time"
                     }
                   }
                   size=Heading.H5
                 />
               | _ => <LoadingCensorBar width=100 height=15 />
               }}
            </div>
            {switch (proposalSub) {
             | Data({depositEndTime, votingEndTime, status}) =>
               <Timestamp
                 size=Text.Lg
                 time={
                   switch (status) {
                   | Deposit => depositEndTime
                   | Voting
                   | Passed
                   | Rejected
                   | Failed => votingEndTime
                   }
                 }
               />
             | _ => <LoadingCensorBar width={isMobile ? 120 : 270} height=15 />
             }}
          </Col.Grid>
          {switch (proposalSub) {
           | Data({status, turnout}) =>
             switch (status) {
             | Deposit => React.null
             | Voting
             | Passed
             | Rejected
             | Failed =>
               <Col.Grid col=Col.Four colSm=Col.Five>
                 <Heading value="Turnout" size=Heading.H5 marginBottom=8 />
                 <Text value={turnout |> Format.fPercent(~digits=2)} size=Text.Lg />
               </Col.Grid>
             }
           | _ =>
             <Col.Grid col=Col.Four colSm=Col.Five>
               <LoadingCensorBar width=100 height=15 mb=8 />
               <LoadingCensorBar width=50 height=15 />
             </Col.Grid>
           }}
        </Row.Grid>
      </div>
    </Col.Grid>;
  };
};

[@react.component]
let make = () => {
  let pageSize = 10;
  let proposalsSub = ProposalSub.getList(~pageSize, ~page=1, ());
  let proposalsCountSub = ProposalSub.count();
  let allSub = Sub.all2(proposalsSub, proposalsCountSub);

  <Section>
    <div className=CssHelper.container>
      <Row.Grid alignItems=Row.Center marginBottom=40 marginBottomSm=24>
        <Col.Grid col=Col.Twelve> <Heading value="All Proposals" size=Heading.H2 /> </Col.Grid>
      </Row.Grid>
      //TODO: It will be shown after there are more proposals
      // {switch (allSub) {
      //  | Data((_, proposalsCount)) =>
      //    <Heading value={proposalsCount->string_of_int ++ " In total"} size=Heading.H3 />
      //  | _ => <LoadingCensorBar width=65 height=21 />
      //  }}
      <Row.Grid>
        {switch (allSub) {
         | Data((proposals, _)) =>
           proposals
           ->Belt_Array.mapWithIndex((i, e) =>
               <ProposalCard
                 key={i |> string_of_int}
                 reserveIndex=i
                 proposalSub={Sub.resolve(e)}
               />
             )
           ->React.array
         | _ =>
           Belt_Array.make(pageSize, ApolloHooks.Subscription.NoData)
           ->Belt_Array.mapWithIndex((i, noData) =>
               <ProposalCard key={i |> string_of_int} reserveIndex=i proposalSub=noData />
             )
           ->React.array
         }}
      </Row.Grid>
    </div>
  </Section>;
};
