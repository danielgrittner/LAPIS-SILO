const { server, headerToHaveDataVersion } = require('./common');
const fs = require('fs');
const { expect } = require('chai');

const queriesPath = __dirname + '/queries';
const queryTestFiles = fs.readdirSync(queriesPath);
const invalidQueriesPath = __dirname + '/invalidQueries';
const invalidQueryTestFiles = fs.readdirSync(invalidQueriesPath);

describe('The /query endpoint', () => {
  const testCases = queryTestFiles.map(file => JSON.parse(fs.readFileSync(`${queriesPath}/${file}`)));

  testCases.forEach(testCase =>
    it('should return data for the test case ' + testCase.testCaseName, async () => {
      const response = await server.post('/query').send(testCase.query);
      expect(response.body.queryResult).to.deep.equal(testCase.expectedQueryResult);
      expect(200);
      expect('Content-Type', 'application/json');
      expect(headerToHaveDataVersion);
    })
  );

  it(' - the query test cases should have unique names', () => {
    const testCaseNames = testCases.map(testCase => testCase.testCaseName);
    const uniqueTestCaseNames = [...new Set(testCaseNames)];

    expect(testCaseNames, 'Found non-unique test case names').to.deep.equal(uniqueTestCaseNames);
  });

  const invalidQueryTestCases = invalidQueryTestFiles.map(file =>
    JSON.parse(fs.readFileSync(`${invalidQueriesPath}/${file}`))
  );
  invalidQueryTestCases.forEach(testCase =>
    it('should return the expected error for the test case ' + testCase.testCaseName, async () => {
      const response = await server
        .post('/query')
        .send(testCase.query)
        .expect(400)
        .expect('Content-Type', 'application/json');
      return expect(response.body).to.deep.equal(testCase.expectedError);
    })
  );

  it(' - the invalid query test cases should have unique names', () => {
    const testCaseNames = invalidQueryTestCases.map(testCase => testCase.testCaseName);
    const uniqueTestCaseNames = [...new Set(testCaseNames)];

    expect(testCaseNames, 'Found non-unique invalid query test case names').to.deep.equal(
      uniqueTestCaseNames
    );
  });

  it('should return a method not allowed response when sending a GET request', done => {
    server
      .get('/query')
      .send()
      .expect(405)
      .expect('Content-Type', 'application/json')
      .expect({
        error: 'Method not allowed',
        message: 'GET is not allowed on resource /query',
      })
      .end(done);
  });

  it('should return a bad request response when POSTing an invalid JSON', done => {
    server
      .post('/query')
      .send('{ not a valid json')
      .expect(400)
      .expect('Content-Type', 'application/json')
      .expect({
        error: 'Bad request',
        message:
          'The query was not a valid JSON: [json.exception.parse_error.101] ' +
          'parse error at line 1, column 4: syntax error while parsing object key - invalid literal; ' +
          "last read: '{ no'; expected string literal",
      })
      .end(done);
  });

  it('should return a bad request response when POSTing a JSON without filter and action', done => {
    server
      .post('/query')
      .send({ someJson: 'but missing expected properties' })
      .expect(400)
      .expect('Content-Type', 'application/json')
      .expect({
        error: 'Bad request',
        message: 'Query json must contain filterExpression and action.',
      })
      .end(done);
  });

  it('should return a bad request response when POSTing an invalid filter type', done => {
    server
      .post('/query')
      .send({
        action: {
          type: 'invalid action',
        },
        filterExpression: {
          type: 'invalid filter type',
        },
      })
      .expect(400)
      .expect('Content-Type', 'application/json')
      .expect({
        error: 'Bad request',
        message: "Unknown object filter type 'invalid filter type'",
      })
      .end(done);
  });
});
